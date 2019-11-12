# Skills-Record

you can download different versions of Anaconda in https://repo.continuum.io/

The mirror source of Tsinghua is available again

```
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/
conda config --set show_channel_urls yes
```

install pytorch with specified version
```
conda install pytorch=1.x cudatoolkit=x.0
```
install opencv
```
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple opencv-python

```

强烈推荐使用中科院arxiv的镜像地址：http://xxx.itp.ac.cn

具体使用方法：把要访问 arxiv 链接中的域名从 https://arxiv.org 换成 http://xxx.itp.ac.cn 

