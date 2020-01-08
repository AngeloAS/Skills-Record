# Skills-Record

you can download different versions of Anaconda in https://repo.continuum.io/

The mirror source of Tsinghua is available again

```
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud/pytorch/
conda config --set show_channel_urls yes
```

install pytorch with specified version
```
conda install pytorch=1.x cudatoolkit=x.0
```
install opencv
```
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple opencv-python
pip install opencv-contrib-python -i https://pypi.douban.com/simple

```
final choice: download *.tar.bz file and use follow order
```
conda install --use-local *.tar.bz
```


pip install accelerate ways

```
pip install name_of_file -i https://pypi.douban.com/simple
```

accelerate arxiv speed
'''
http://xxx.itp.ac.cn/pdf/1911.08651.pdf
'''


