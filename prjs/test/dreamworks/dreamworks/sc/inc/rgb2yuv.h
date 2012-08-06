#ifndef INC_H 
#define INC_H 
 
#define Uint8		unsigned char 
#define Int8		char 
#define Uint16		unsigned short 
#define Int16		short 
#define Uint32		unsigned int 
#define Int32		int 
 
 
#define Y_R_IN			0.257 
#define Y_G_IN			0.504 
#define Y_B_IN			0.098 
#define Y_ADD_IN		16 
 
#define U_R_IN			0.148 
#define U_G_IN			0.291 
#define U_B_IN			0.439 
#define U_ADD_IN		128 
 
#define V_R_IN			0.439 
#define V_G_IN			0.368 
#define V_B_IN			0.071 
#define V_ADD_IN		128 
 
#define SCALEBITS_IN	8 
#define FIX_IN(x)		((Uint16) ((x) * (1L<<SCALEBITS_IN) + 0.5)) 
 
 
void rgb_to_yv12_c(Uint8 * y_out, Uint8 * src, int width, int height, int y_stride); 
void rgb_to_yv12_init(); 
 
#endif