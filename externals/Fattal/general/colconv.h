#ifndef COLCONV_H
#define COLCONV_H


// [ X ] [ 0.412453  0.35758   0.180423 ] [ R709 ] 
// [ Y ]=[ 0.212671  0.71516   0.072169 ]*[ G709 ] 
// [ Z ] [ 0.019334  0.119193  0.950227 ] [ B709 ] 
void RGB709toXYZ(float rgb[], float xyz[]) {
  const float mat[3][3] =
  {{ 0.412453, 0.35758 , 0.180423},
   { 0.212671, 0.71516 , 0.072169},
   { 0.019334, 0.119193, 0.950227}};
  
  xyz[0] = mat[0][0] * rgb[0] + mat[0][1] * rgb[1] + mat[0][2] * rgb[2];
  xyz[1] = mat[1][0] * rgb[0] + mat[1][1] * rgb[1] + mat[1][2] * rgb[2];
  xyz[2] = mat[2][0] * rgb[0] + mat[2][1] * rgb[1] + mat[2][2] * rgb[2];
}


// [ R709 ] [ 3.240479 -1.53715  -0.498535 ] [ X ] 
// [ G709 ]=[-0.969256  1.875991  0.041556 ]*[ Y ] 
// [ B709 ] [ 0.055648 -0.204043  1.057311 ] [ Z ] 
void XYZtoRGB709(float xyz[], float rgb[]) {
  const float mat[3][3] =
  {{ 3.240479,-1.53715 ,-0.498535},
   {-0.969256, 1.875991, 0.041556},
   { 0.055648,-0.204043, 1.057311}};

  rgb[0] = mat[0][0] * xyz[0] + mat[0][1] * xyz[1] + mat[0][2] * xyz[2];
  rgb[1] = mat[1][0] * xyz[0] + mat[1][1] * xyz[1] + mat[1][2] * xyz[2];
  rgb[2] = mat[2][0] * xyz[0] + mat[2][1] * xyz[1] + mat[2][2] * xyz[2];
}

void XYZtoCIELAB(float xyz[], float lab[]) {
  float refwhite[3] = { 1, 1, 1 };
  float XYZn[3]={1,1,1};
  
  RGB709toXYZ(refwhite, XYZn);

  lab[0] = -16 + 116 * pow(xyz[1] / XYZn[1], 1./3);
  lab[1] = 500 * (pow(xyz[0] / XYZn[0], 1./3.) - pow(xyz[1] / XYZn[1], 1./3.));
  lab[2] = 200 * (pow(xyz[1] / XYZn[1], 1./3.) - pow(xyz[2] / XYZn[2], 1./3.));
}

void CIELABtoXYZ(float lab[], float xyz[]) {
  float refwhite[3] = { 1, 1, 1 };
  float XYZn[3]={1,1,1};
  
  RGB709toXYZ(refwhite, XYZn);

  xyz[0] = XYZn[0] * pow((lab[0] + 16) / 116 + lab[1] / 500, 3);
  xyz[1] = XYZn[1] * pow((lab[0] + 16) / 116, 3);
  xyz[2] = XYZn[2] * pow((lab[0] + 16) / 116 - lab[2] / 200, 3);
}

void RGBtoLAB(float rgb[], float lab[]) {
  float xyz[3];
  RGB709toXYZ(rgb, xyz);
  XYZtoCIELAB(xyz, lab);
}

void LABtoRGB(float lab[], float rgb[]) {
  float xyz[3];
  CIELABtoXYZ(lab, xyz);
  XYZtoRGB709(xyz, rgb);
}


#endif
