# 软渲染 #

----------
之前看过DX的龙书，对软渲染的流程有一定了解，对光栅化部分的介绍很简略，所以大部分问题出现在这里。代码部分采用了DX的框架，一开始没考虑数学库的性能问题，主要时间花在性能优化、细节处理以及BUG修复上。

----------

## 主要实现 ##
- 立方体沿X/Y轴旋转，设置相机以及移动
- BinPhong光照
- 纹理双线性插值
- back-culling，虽然是可选，但是没写性能很难达标
- Z-buffer，但是因为性能关系，性能测试的时候关了
- 远近平面裁剪，验收的时候没有实现，后面补上的
- 近视角全屏幕不旋转三面绘制稳定在70+FPS
- 近视角全屏幕旋转平均在70FPS，偶尔会跳到65FPS
- 上述性能前提，CPU不被其他程序占用
- 上述性能配置：CPU i5 6500 3.3GHz

----------

## 光栅化优化考虑 ##
- 包围盒算法(更便于openMP并行加速，schedule(dynamic)，尽可能利用CPU)
- 点在三角形内判断，利用重心坐标，判断都大于等于0.0f即可，不需要多余的小于等于1.0f判断
- 计算重心坐标，不必计算每一个点的重心坐标，只需要一开始计算其中一个点的重心坐标以及重心坐标在二维平面的梯度，然后根据点坐标计算对应重心坐标即可。
- 扫描包围盒每一行的时候，如果前一个点在三角形内，后一个点不在三角形内，则可以直接进入下一行的绘制。
- 两个三角形之间漏点以及重复边问题，我是通过先绘制一个三角形的所有像素，然后遇到其他三角形也要绘制已近绘制过的像素则不进行绘制（类似zBuffer），后来明确left-top规则，主要考虑每一个点的法向量需要特定的三角形确定(非曲面)

----------


## 光照优化考虑 ##
- 计算镜面光的时候，由于指数是整数，利用O(logN)的pow算法优化，不考虑gamma校正的浮点数指数

----------

## 遇到问题 ##
- 透视校正，考虑不仔细，一开始z坐标用了投影转换后归一化的z导致校正不正确，应该用视角空间正确的z值
- 投影时视锥体的对称性，会将视角前后两个视锥体都投影到投影坐标，出现了近平面裁剪以后再过去出现“视角反向”的情况，后来在视角空间对三角形三个顶点的z做了裁剪处理
- win32的画图用了BitBlt函数，原理是直接用一个屏幕大小的buffer去填充，但是我的机子在旋转以及视角移动的时候会出现画面失真问题（类似前后帧画面错乱），而在倪骏扬的电脑上跑并没有出现这个问题，这个问题暂时还没解决，我这里连续调用两次BitBlt画图函数重复画图，所以上述性能实际上会提高2~3FPS。

----------
## 更多优化 ##
- 计算世界坐标以及视角坐标的时候，最后的（0,0,0,1）一列不需要计算，可以省去空间与时间，DX对这两个矩阵传输前进行转置也是为了这个。
