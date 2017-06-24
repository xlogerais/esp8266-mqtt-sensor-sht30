/*
    Utility function for use in printing floats into buffers like sprintf(...

    The library function dtostrf is the basis of this utility. It does a lot of work
    in converting floats to strings but what makes it a bit irritating is that it needs
    a buffer to write the float into which makes it very cumbersome in in-line code -
    just rewrite the sprintf example below using dtostrf!

    A collection of static char buffers is maintained in the function so that they are used
    consequetively with each call. A single buffer cannot be used when the function is used
    as a parameter to a call like sprintf in the example/test code below. This is simply because
    the compiler writes the code so that all the calls to fs2 are made before passing the
    results of the calls to the routine sprintf.

    This function does hard work of cycling through the internal buffers. The default
    number of buffers is 10 so a sprintf, for example, with more than 10 calls to fs2 will
    overwrite the the beginning of the buffer. Avoid this by making a subsequent call to sprintf.

    Please feel free to use this as you wish... I'm sure some enterprising coder will improve it :)

    Alan Boother 2013/07/03
*/

// float to string
char *f2s(float f, int p){
  char * pBuff;                         // use to remember which part of the buffer to use for dtostrf
  const int iSize = 10;                 // number of bufffers, one for each float before wrapping around
  static char sBuff[iSize][20];         // space for 20 characters including NULL terminator for each float
  static int iCount = 0;                // keep a tab of next place in sBuff to use
  pBuff = sBuff[iCount];                // use this buffer
  if(iCount >= iSize -1){               // check for wrap
    iCount = 0;                         // if wrapping start again and reset
  }
  else{
    iCount++;                           // advance the counter
  }
  return dtostrf(f, 0, p, pBuff);       // call the library function
}
