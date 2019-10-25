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
using namespace std;
using namespace cv;

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


	/*read images*/
	std::vector<cv::String> filenames;
	cv::String folder = "D:\\data\\tanglin\\maskrcnn_c++\\川大原图\\Picture2"; //images folder
	cv::String save_folder = "D:\\data\\tanglin\\maskrcnn_c++\\result\\川大\\"; //save folder
	cv::glob(folder, filenames);

	for (size_t i = 0; i<filenames.size(); ++i) {
		std::cout << filenames[i] << std::endl;
		auto img_ = cv::imread(filenames[i], cv::IMREAD_COLOR);
		auto image_ori = img_;
		float width_ori = img_.cols;
		float height_ori = img_.rows;
		cv::Mat img(960, 1312, CV_8UC3);
		cv::resize(img_, img, img.size(), 0, 0, cv::INTER_LINEAR);  // must be bilinear interpolation,it fits our works more
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
		string filename = filenames[i];;
		vector<string> filename_str = split(filename, '\\');

		vector<float>ratio;
		ratio.push_back((width_ori) / double(1312)); //picture resize ratio
		ratio.push_back((height_ori) / double(960));

		if (!error_flag) {
			auto result = module->forward(inputs);
			auto tuple = (result.toTuple())->elements();
			int standNum = 0, sitNum = 0, lieNum = 0;

			res = tuple;
			torch::Tensor box = res[0].toTensor();
			torch::Tensor kind = res[1].toTensor();
			torch::Tensor score = res[2].toTensor();

			int number = box.size(0); //the people number for the image

			for (int j = 0; j<number; j++) {
				auto x1 = (box[j][0]).item().toFloat();
				auto y1 = (box[j][1]).item().toFloat();
				auto x2 = (box[j][2]).item().toFloat();
				auto y2 = (box[j][3]).item().toFloat();
				float xmax, xmin, ymax, ymin;
				int X1, X2, Y1, Y2;
				xmin = x1;
				xmax = x2;
				ymin = y1;
				ymax = y2;

				X1 = int(xmin * ratio[0]);
				X2 = int(xmax * ratio[0]);
				Y1 = int(ymin * ratio[1]);
				Y2 = int(ymax * ratio[1]);
				if (kind[j].item().toFloat() == 1)
				{
					rectangle(image_ori, cvPoint(X1, Y1), cvPoint(X2, Y2), CV_RGB(0, 255, 0), 2, 8, 0);
					standNum++;
				}
				else if (kind[j].item().toFloat() == 2)
				{
					rectangle(image_ori, cvPoint(X1, Y1), cvPoint(X2, Y2), CV_RGB(0, 0, 255), 2, 8, 0);
					sitNum++;
				}
				else if (kind[j].item().toFloat() == 3)
				{
					rectangle(image_ori, cvPoint(X1, Y1), cvPoint(X2, Y2), CV_RGB(255, 0, 0), 2, 8, 0);
					lieNum++;
				}
				


			}
			cout << "peopleCount = " << standNum + sitNum + lieNum << "\nstanding = " << standNum << "\nsitting = " << sitNum << "\nlying = " << lieNum << endl;
			imwrite(save_folder + filename_str[filename_str.size() - 1], image_ori);

		}

		else {
			imwrite(save_folder + filename_str[filename_str.size() - 1], image_ori);
			cout << "Error dectition!!" << endl;
			//imshow("test", image_ori);
			//waitKey(300);
			//imwrite("D:\\data\\tanglin\\maskrcnn_c++\\result\\川大\\" +b[b.size()-1], image_ori);
		}
		//cout << "model can be caculate" << endl;

	}

 
	
	getchar();
 	return 0;
  
}
