enum{ STRATIFIED, REGULAR, WHITENOISE, GLOBAL_DATSAMPLER };

#define NBBOUNCES 1
#define IMGRES 512
#define NBDIM 2+NBBOUNCES*3

int mapping[NBDIM] =
{ GLOBAL_DATSAMPLER, //image plane (2D)
  GLOBAL_DATSAMPLER, //depth of field (2D)
  //Bounce 1
  GLOBAL_DATSAMPLER, //Light point (2D)
  GLOBAL_DATSAMPLER, //Light scattering (2D)
  GLOBAL_DATSAMPLER //BSDF (Bounce direction) (2D)
  //Bounce 2
//   OURS, //Light point (2D)
//   STRATIFIED, //Light scattering (2D)
//   STRATIFIED //BSDF (Bounce direction) (2D)
};

//randomization between pixels using cranley patterson
int randomizing[NBDIM] =
{ 0, //image plane (2D)
  0, //depth of field (2D)
  //Bounce 1
  0, //Light point (2D)
  0, //Light scattering (2D)
  0 //BSDF (Bounce direction) (2D)
  //Bounce 2
//   0, //Light point (2D)
//   0, //Light scattering (2D)
//   0 //BSDF (Bounce direction) (2D)
};

//shuffle between pairs of dimensions
int shuffle[NBDIM] =
{ 0, //image plane (2D)
  0, //depth of field (2D)
  //Bounce 1
  0, //Light point (2D)
  0, //Light scattering (2D)
  0 //BSDF (Bounce direction) (2D)
  //Bounce 2
//   0, //Light point (2D)
//   0, //Light scattering (2D)
//   0 //BSDF (Bounce direction) (2D)
};
