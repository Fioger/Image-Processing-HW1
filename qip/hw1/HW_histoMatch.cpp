// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
// Written by: ADD YOUR NAMES HERE, 2016
//
void
HW_histoMatch(ImagePtr I1, ImagePtr Ilut, ImagePtr I2)
{
  int i, p, R;
  int left[MXGRAY], right[MXGRAY];
  int total, Hsum, Havg, h1[MXGRAY];
  double scale;

	IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	total = w * h;

  int type;
  ChannelPtr<uchar> in, out;
  ChannelPtr<int> h2;
  IP_getChannel(Ilut, 0, h2, type);       /* target histogram */

  /* normalize h2 to conform with dimensions of I1 */
  double prev_count = 0.0;
  double curr_count = 0.0;
  for(i=Havg=0; i<MXGRAY; i++) Havg += h2[i];
  scale = (double) total / Havg;
  if (scale != 1) for (i = 0; i < MXGRAY; i++){
    /* If the scale is not an integer, then h2 will be missing some pixels, 
    which it automatically sets to an intensity of 0. Keep track of current count 
    and previous count as doubles to prevent missing pixels. */
	  curr_count += (double) h2[i] * scale;
	  h2[i] *= (double) scale; 
    /* When adding decimals, they eventually add up to a whole number.
    So when you add the integer version of decimals up, you end up losing 
    some pixels, which is what is checked here. When decimals add up to a 
    whole number, make sure to keep track of that pixel.*/
	  if (FLOOR(curr_count) - FLOOR(prev_count) > FLOOR(h2[i])){
		  h2[i] += 1;
	  }
	  prev_count = curr_count;
  }

  // for each channel
  for(int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {
    IP_getChannel(I2, ch, out, type);

    for(i=0; i<MXGRAY; i++) h1[i] = 0;    /* clear histogram */
    for(i=0; i<total ; i++) h1[in[i]]++;  /* eval histogram */

    R = 0;
    Hsum = 0;
    /* evaluate remapping of all input gray levels;
    Each input gray value maps to an interval of valid output values.
    The endpoints of the intervals are left[] and right[] */
    for(i=0; i<MXGRAY; i++) {
      left[i] = R;                        /* left end of interval */
      Hsum += h1[i];
      /* cumulative value for interval */
      while(Hsum>h2[R] && R<MXGRAY-1) {   /* compute width of interval */
        Hsum -= h2[R];                    /* adjust Hsum as interval widens */
        R++;                              /* update */
      }
      right[i] = R;                       /* init right end of interval */
    }

    /* clear h1 and reuse it below */
    for(i=0; i<MXGRAY; i++) h1[i] = 0;
    /* visit all input pixels */
    for(i=0; i<total; i++) {
      p = left[in[i]];
      /* while mapping doesn't satisfy h2;
      Compare 1 plus the output histogram entry because we are evaluating
      where to place an additional intensity that hasn't been put into the
      histogram yet. */
      while ((p < MXGRAY) && (h1[p] + 1) > h2[p]) {
        /* widen interval for to allow for spillover */
        left[in[i]] = ++p;
      }
      out[i] = p;   /* assign intensity value */
      h1[p]++;      /* update output histogram */
    }
  }
}