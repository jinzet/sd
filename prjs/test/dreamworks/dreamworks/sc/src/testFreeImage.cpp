
#include <iostream>
#include <cassert>
#include "FreeImage.h"

#ifdef _DEBUG
#pragma comment(lib,"FreeImaged.lib")
#else
#pragma comment(lib,"FreeImage.lib")
#endif

#define show(a) std::cout<< a <<std::endl

int main_test(int argc,char *argv)
{

   //初始化FreeImage

   FreeImage_Initialise(TRUE);

   //默认分配一块内存存储影像，其中bpp表示图像深度

   //FIBITMAP *bitmap = FreeImage_AllocateT(FIT_BITMAP,Width,height,bpp,

   //red_mask,green_mask,blue_mask);

   //最后一个参数用来改变函数的行为或激活位图插件的一种特性，每个插件有他自己的参数集

   //FreeImage_LoadU参数都是一样，不过只在win32下起作用

   FIBITMAP * bmp = FreeImage_Load(FIF_BMP,"cs5.BMP",BMP_DEFAULT);

   //FreeImage_SaveU和上面的一样

   if( bmp)
   {
  //FreeImage_Save(FIF_BMP, bmp, "whtm.bmp", BMP_DEFAULT);
   }

   //获取影像的宽高，都以像素为单位
   int Width = FreeImage_GetWidth(bmp);
   int Height = FreeImage_GetHeight(bmp);

   //获取影像的byte宽度，不对，我上面运行与Width相等
   int ByteWidth = FreeImage_GetLine(bmp);

   //返回每个像素使用灰度值
   show(FreeImage_GetBPP(bmp));

   //返回数据类型
   show(FreeImage_GetImageType(bmp));

   //返回调色板的大小
   show(FreeImage_GetColorsUsed(bmp));

   show(Width<<" "<<Height<<" "<<ByteWidth);

 //assert(ByteWidth == Width*8 );

   show(FreeImage_GetDIBSize(bmp));

   //有调用load就有unload，否则就会内存泄漏
   FreeImage_Unload(bmp);

   FreeImage_DeInitialise();

   return 0;
}