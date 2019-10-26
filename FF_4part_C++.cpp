#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <torch/script.h>
#include <dlfcn.h>
#include <typeinfo>
#include <string>
#include <cstring>
#include <vector>
#include <windows.h>
#include <time.h>
#include <tuple>
#include"gpu_nms.h"
using namespace std;
using namespace cv;
#pragma comment(lib,"./NMS_GPU_9.0.lib")
void _nms(int* keep_out, int* num_out, const float* boxes_host, int boxes_num,
	int boxes_dim, float nms_overlap_thresh, int device_id);

//split string function
vector<string> split(string &str, char ch)
{
	vector<string> res;
	str += ch;
	int start = 0;
	int last = str.find(ch);
	while (last < str.size())
	{
		if (start != last)
			res.push_back(str.substr(start, last - start));
		start = last + 1;
		last = str.find(ch, start);
	}
	return res;
}

typedef struct Bbox
{
	int x1;
	int y1;
	int x2;
	int y2;
	int kind;
	float score;
}Bbox;

bool sort_Bbox(Bbox sc1, Bbox sc2)
{
	return (sc1.score > sc2.score);

}
int main(){
	//const char* customOpsFolder = "D:\\data\\tanglin\\maskrcnn_c++\\dll\\custom_ops_no_opencv.cp36-win_amd64.dll";
	void* custom_op_lib = dlopen("custom_ops_no_opencv.cp36-win_amd64.dll", RTLD_NOW | RTLD_GLOBAL);//load custom ops

	if (custom_op_lib == NULL) {
		cerr << "could not open custom op library: " << dlerror() << endl;
	}

	cout << "load detection algorithm ...." << endl;


	/*load model*/
	std::shared_ptr<torch::jit::script::Module> module = torch::jit::load("D:\\data\\tanglin\\maskrcnn_c++\\model_pose_gpu.pt");
	//torch::jit::script::Module module = torch::jit::load("D:\\data\\tanglin\\maskrcnn_c++\\model_gpu.pt");
	assert(module != nullptr);

	//torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);
	torch::DeviceType device_type;
	//device_type = torch::kCPU;  
	device_type = torch::kCUDA;  //set cuda or cpu
	torch::Device device(device_type, 0);
	module->to(device);

	//cout << "finished load model" << endl;
	int x_in = 1312, y_in = 960;


	/*read images*/
	std::vector<cv::String> filenames;
	cv::String folder = "D:\\data\\tanglin\\maskrcnn_c++\\川大原图\\Picture2"; //images folder
	cv::String save_folder = "D:\\data\\tanglin\\maskrcnn_c++\\result\\川大\\"; //save folder
	cv::glob(folder, filenames);

	for (int i = 0; i<filenames.size(); ++i) {
		std::cout << filenames[i] << std::endl;
		string filename = filenames[i];;
		vector<string> filename_str = split(filename, '\\');
		cv::Mat img_ = cv::imread(filenames[i]);
		cv:Mat image_ori = img_;
		double height_ori = img_.cols;
		double width_ori = img_.rows;
		double hratio = 0.7;
		double wratio = 0.7;
		double ratio[2];
		int keep[300];
		int num_out = 0;
		int num = 0;
		float box_in[600][5]; // x1,y1,x2,y2,score,kind
		float result[300][5];
		memset(keep, 0, sizeof(keep));
		memset(box_in, 0, sizeof(box_in));
		memset(result, 0, sizeof(result));
		vector<Bbox> bsort;
		ratio[0] = (double)(height_ori*hratio) / double(x_in);
		ratio[1] = (double)(width_ori*wratio) / double(y_in);
		cv::Mat img1 = img_(Range(0, (int)(wratio * width_ori)), Range(0, (int)(hratio * height_ori)));
		cv::Mat img2 = img_(Range(0, (int)(wratio * width_ori)), Range(height_ori - (int)(hratio* height_ori), height_ori));
		cv::Mat img3 = img_(Range(width_ori - (int)(wratio*width_ori), width_ori), Range(0, (int)(hratio * height_ori)));
		cv::Mat img4 = img_(Range(width_ori - (int)(wratio*width_ori), width_ori), Range(height_ori - (int)(hratio* height_ori), height_ori));
		std::tuple<cv::Mat, cv::Mat, cv::Mat, cv::Mat> img_4part(img1, img2, img3, img4);
		int standNum = 0, sitNum = 0, lieNum = 0;
		for (int p = 0;p < 4;p++)
		{
			torch::Tensor box;
			torch::Tensor score;
			torch::Tensor kind;
			cv::Mat img(y_in, x_in, CV_8UC3);
			if (p == 0)
			{
				cv::resize(img1, img, img.size(), 0, 0, cv::INTER_LINEAR);  // must be bilinear interpolation,it fits our works more
				auto input_ = torch::tensor(at::ArrayRef<uint8_t>(img.data, img.rows * img.cols * 3)).view({ img.rows, img.cols, 3 });
				/* data -> model */
				std::vector<torch::jit::IValue> inputs;
				inputs.push_back(input_);
				bool error_flag;  //runtime error flag
				try {
					auto result_tr = module->forward(inputs);
					error_flag = false;
				}
				catch (std::runtime_error) {
					error_flag = true;
				}
				std::vector<c10::IValue, std::allocator<c10::IValue>> res; //res must be this type
				auto result = module->forward(inputs);
				auto tuple = (result.toTuple())->elements();
				res = tuple;
				box = res[0].toTensor();
				kind = res[1].toTensor();
				score = res[2].toTensor();
				for (int tt = 0;tt < box.size(0);tt++)
				{
					Bbox btmp;
					btmp.x1 = box[tt][0].item().toInt() * ratio[0];
					btmp.y1 = box[tt][1].item().toInt() * ratio[1];
					btmp.x2 = box[tt][2].item().toInt() * ratio[0];
					btmp.y2 = box[tt][3].item().toInt() * ratio[1];
					btmp.kind = kind[tt].item().toInt();
					btmp.score = score[tt].item().toFloat();
					bsort.push_back(btmp);
				}
			}
			else if (p == 1)
			{
				cv::resize(img2, img, img.size(), 0, 0, cv::INTER_LINEAR);  // must be bilinear interpolation,it fits our works more
				auto input_ = torch::tensor(at::ArrayRef<uint8_t>(img.data, img.rows * img.cols * 3)).view({ img.rows, img.cols, 3 });
				/* data -> model */
				std::vector<torch::jit::IValue> inputs;
				inputs.push_back(input_);
				bool error_flag;  //runtime error flag
				try {
					auto result_tr = module->forward(inputs);
					error_flag = false;
				}
				catch (std::runtime_error) {
					error_flag = true;
				}
				std::vector<c10::IValue, std::allocator<c10::IValue>> res; //res must be this type
				auto result = module->forward(inputs);
				auto tuple = (result.toTuple())->elements();
				res = tuple;
				box = res[0].toTensor();
				kind = res[1].toTensor();
				score = res[2].toTensor();
				for (int tt = 0;tt < box.size(0);tt++)
				{
					Bbox btmp;
					btmp.x1 = box[tt][0].item().toInt() * ratio[0] + (int)((1 - hratio)*(height_ori));
					btmp.y1 = box[tt][1].item().toInt() * ratio[1];
					btmp.x2 = box[tt][2].item().toInt() * ratio[0] + (int)((1 - hratio)*(height_ori));
					btmp.y2 = box[tt][3].item().toInt() * ratio[1];
					btmp.kind = kind[tt].item().toInt();
					btmp.score = score[tt].item().toFloat();
					bsort.push_back(btmp);
				}
				
			}
			else if (p == 2)
			{
				cv::resize(img3, img, img.size(), 0, 0, cv::INTER_LINEAR);  // must be bilinear interpolation,it fits our works more
				auto input_ = torch::tensor(at::ArrayRef<uint8_t>(img.data, img.rows * img.cols * 3)).view({ img.rows, img.cols, 3 });
				/* data -> model */
				std::vector<torch::jit::IValue> inputs;
				inputs.push_back(input_);
				bool error_flag;  //runtime error flag
				try {
					auto result_tr = module->forward(inputs);
					error_flag = false;
				}
				catch (std::runtime_error) {
					error_flag = true;
				}
				std::vector<c10::IValue, std::allocator<c10::IValue>> res; //res must be this type
				auto result = module->forward(inputs);
				auto tuple = (result.toTuple())->elements();
				res = tuple;
				box = res[0].toTensor();
				kind = res[1].toTensor();
				score = res[2].toTensor();
				for (int tt = 0;tt < box.size(0);tt++)
				{
					Bbox btmp;
					btmp.x1 = box[tt][0].item().toInt() * ratio[0];
					btmp.y1 = box[tt][1].item().toInt() * ratio[1] + (int)((1 - wratio)*(width_ori));
					btmp.x2 = box[tt][2].item().toInt() * ratio[0];
					btmp.y2 = box[tt][3].item().toInt() * ratio[1] + (int)((1 - wratio)*(width_ori));
					btmp.kind = kind[tt].item().toInt();
					btmp.score = score[tt].item().toFloat();
					bsort.push_back(btmp);
				}
			}
			else if (p == 3)
			{
				cv::resize(img4, img, img.size(), 0, 0, cv::INTER_LINEAR);  // must be bilinear interpolation,it fits our works more
				auto input_ = torch::tensor(at::ArrayRef<uint8_t>(img.data, img.rows * img.cols * 3)).view({ img.rows, img.cols, 3 });
				/* data -> model */
				std::vector<torch::jit::IValue> inputs;
				inputs.push_back(input_);
				bool error_flag;  //runtime error flag
				try {
					auto result_tr = module->forward(inputs);
					error_flag = false;
				}
				catch (std::runtime_error) {
					error_flag = true;
				}
				std::vector<c10::IValue, std::allocator<c10::IValue>> res; //res must be this type
				auto result = module->forward(inputs);
				auto tuple = (result.toTuple())->elements();
				res = tuple;
				box = res[0].toTensor();
				kind = res[1].toTensor();
				score = res[2].toTensor();
				for (int tt = 0;tt < box.size(0);tt++)
				{
					Bbox btmp;
					btmp.x1 = box[tt][0].item().toInt() * ratio[0] + (int)((1 - hratio)*(height_ori));
					btmp.y1 = box[tt][1].item().toInt() * ratio[1] + (int)((1 - wratio)*(width_ori));
					btmp.x2 = box[tt][2].item().toInt() * ratio[0] + (int)((1 - hratio)*(height_ori));
					btmp.y2 = box[tt][3].item().toInt() * ratio[1] + (int)((1 - wratio)*(width_ori));
					btmp.kind = kind[tt].item().toInt();
					btmp.score = score[tt].item().toFloat();
					bsort.push_back(btmp);
				}
			}
		}
		cout << bsort.size() << endl;
		sort(bsort.begin(),bsort.end(),sort_Bbox);
		cout << bsort.size() << endl;
		num = 0;
		for (vector<Bbox>::iterator tt = bsort.begin();tt != bsort.end();tt++)
		{
			box_in[num][0] = tt->x1;
			box_in[num][1] = tt->y1;
			box_in[num][2] = tt->x2;
			box_in[num][3] = tt->y2;
			box_in[num][4] = tt->score;
			num++;
		}
		_nms(keep, &num_out, (const float*)box_in, num, 5, 0.3, 0);
		num = 0;
		cout << num_out << endl;
		for (int nn = 0;nn < num_out;++nn) {
			result[num][0] = box_in[keep[nn]][0];
			result[num][1] = box_in[keep[nn]][1];
			result[num][2] = box_in[keep[nn]][2];
			result[num][3] = box_in[keep[nn]][3];
			result[num][4] = bsort[keep[nn]].kind;
			++num;
		}

		for (int j = 0; j<num_out; j++) {
			if (result[j][4] == 1)
			{
				rectangle(img_, cvPoint(result[j][0], result[j][1]), cvPoint(result[j][2], result[j][3]), CV_RGB(0, 255, 0), 2, 8, 0);
				standNum++;
			}
			else if (result[j][4] == 2)
			{
				rectangle(img_, cvPoint(result[j][0], result[j][1]), cvPoint(result[j][2], result[j][3]), CV_RGB(0, 0, 255), 2, 8, 0);
				sitNum++;
			}
			else if (result[j][4] == 3)
			{
				rectangle(img_, cvPoint(result[j][0], result[j][1]), cvPoint(result[j][2], result[j][3]), CV_RGB(255, 0, 0), 2, 8, 0);
				lieNum++;
			}
		}
		cout << "peopleCount = " << standNum + sitNum + lieNum << "\nstanding = " << standNum << "\nsitting = " << sitNum << "\nlying = " << lieNum << endl;
		cv::imwrite(save_folder + filename_str[filename_str.size() - 1], img_);
	}
	getchar();
 	return 0;

  
}
