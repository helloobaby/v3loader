这个项目本意是偷取一个csgo外挂注入器的DLL，然后尝试自己注入的，可惜失败了，

原因是不清楚那个注入器的注入方式，一时想不到咋继续搞了，就直接搁置了这个项目。

用的工具是[ept hook](https://github.com/helloobaby/Intel-Hv)。可以拦截写内存操作，然后转出为两个fix文件，第一个是原始DLL的PE头，第二个是一段shellcode，没分析出来具体是干嘛的
