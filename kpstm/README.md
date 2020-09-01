
## seismic prestack time migration

积分法时间偏移


## 计算过程
- 确定观测系统
- 对数据道进行加密
- 计算根据速度场计算旅行时参数
- 批量进行道数据IO读入

## 第三方库
- [MS-MPI](https://github.com/Microsoft/Microsoft-MPI)
- [GLOG](https://github.com/google/glog)
- [GFLAGS](https://github.com/gflags/gflags)
- [json](https://github.com/nlohmann/json)
- BOOST
- EIGEN

## MS-MPI
- [How to compile and run a simple MS-MPI program](https://docs.microsoft.com/en-us/archive/blogs/windowshpc/how-to-compile-and-run-a-simple-ms-mpi-program)

## 问题汇总
- VS2019链接glob.lib需要定义宏GOOGLE_GLOG_DLL_DECL，否者LINK错误

## 网络资源
- [glog的编译和使用](https://www.cnblogs.com/xl2432/p/11825966.html)
- [How To Use Google Logging Library (glog)](https://hpc.nih.gov/development/glog.html)
- [如何使用glog](http://www.programgo.com/article/7481399830/;jsessionid=1AC8ADAE01EC6F1B7EBBBD5AD12B5722)