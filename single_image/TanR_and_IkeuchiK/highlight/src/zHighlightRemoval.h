// zHighlightRemoval: to remove highlights or specularities
// by: robby

/**------------------------------
   reference: 
   "separating reflection components of textured surfaces using a single image"
   by Robby T. Tan, Katsushi Ikeuchi,
   IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI),
   27(2), pp.179-193, February, 2005
/*------------------------------*/


#ifndef __Z_HIGHLIGHT_REMOVAL_H__
#define __Z_HIGHLIGHT_REMOVAL_H__


#include "zGlobal.h"

class zHighlightRemoval 
{
  
 protected:
  int zRemoveHighlights(zArray2D<s_rgbi> &img, zArray2D<s_rgbi> &diff, std::string fname);
  int zSpecularFreeImage(zArray2D<s_rgbi> &img, zArray2D<s_rgbi> &diff);

  int zResetLabels(zArray2D<s_rgbi> &src);
  int zSpecular2Diffuse(s_rgbi &iro, float maxChroma);
  int zInit(zArray2D<s_rgbi> &src, zArray2D<s_rgbi> &sfi, float epsilon);
  int zIteration(zArray2D<s_rgbi> &src,zArray2D<s_rgbi> &sfi, float epsilon, bool save, int n);

  void save_needed(zArray2D<s_rgbi> &src, bool mark, string name);
  void clean(zArray2D<s_rgbi> &src);

 public:
  zHighlightRemoval(basic_string<char> fname);
  virtual ~zHighlightRemoval();
  
};

#endif 
