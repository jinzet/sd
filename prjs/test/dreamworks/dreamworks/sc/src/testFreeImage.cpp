
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

   //��ʼ��FreeImage

   FreeImage_Initialise(TRUE);

   //Ĭ�Ϸ���һ���ڴ�洢Ӱ������bpp��ʾͼ�����

   //FIBITMAP *bitmap = FreeImage_AllocateT(FIT_BITMAP,Width,height,bpp,

   //red_mask,green_mask,blue_mask);

   //���һ�����������ı亯������Ϊ�򼤻�λͼ�����һ�����ԣ�ÿ����������Լ��Ĳ�����

   //FreeImage_LoadU��������һ��������ֻ��win32��������

   FIBITMAP * bmp = FreeImage_Load(FIF_BMP,"cs5.BMP",BMP_DEFAULT);

   //FreeImage_SaveU�������һ��

   if( bmp)
   {
  //FreeImage_Save(FIF_BMP, bmp, "whtm.bmp", BMP_DEFAULT);
   }

   //��ȡӰ��Ŀ�ߣ���������Ϊ��λ
   int Width = FreeImage_GetWidth(bmp);
   int Height = FreeImage_GetHeight(bmp);

   //��ȡӰ���byte��ȣ����ԣ�������������Width���
   int ByteWidth = FreeImage_GetLine(bmp);

   //����ÿ������ʹ�ûҶ�ֵ
   show(FreeImage_GetBPP(bmp));

   //������������
   show(FreeImage_GetImageType(bmp));

   //���ص�ɫ��Ĵ�С
   show(FreeImage_GetColorsUsed(bmp));

   show(Width<<" "<<Height<<" "<<ByteWidth);

 //assert(ByteWidth == Width*8 );

   show(FreeImage_GetDIBSize(bmp));

   //�е���load����unload������ͻ��ڴ�й©
   FreeImage_Unload(bmp);

   FreeImage_DeInitialise();

   return 0;
}