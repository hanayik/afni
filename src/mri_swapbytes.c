/*****************************************************************************
   Major portions of this software are copyrighted by the Medical College
   of Wisconsin, 1994-2000, and are released under the Gnu General Public
   License, Version 2.  See the file README.Copyright for details.
******************************************************************************/
   
#include "mrilib.h"

/*** 7D SAFE ***/

#define SWAB16(x) ( ( ((x)&0x00ffU)<<8 ) | ( ((x)&0xff00U)>>8 ) )

void mri_swapbytes( MRI_IMAGE *im )
{
   register int ii , npix ;

WHOAMI ; IMHEADER(im) ;

   if( im->kind != MRI_short ){
      fprintf( stderr , "mri_swapbytes called with non-short image kind\n" ) ;
      MRI_FATAL_ERROR ;
   }

   npix = im->nvox ;

   for( ii=0 ; ii < npix ; ii++ )
      im->im.short_data[ii] = SWAB16( im->im.short_data[ii] ) ;

   return ;
}

/*---------------------------------------------------------------------
   Routines to swap byte arrays in pairs and tetrads -- 14 Sep 1998
-----------------------------------------------------------------------*/

typedef struct { unsigned char a,b ; } twobytes ;

void swap_twobytes( int n , void * ar )
{
   register int ii ;
   register twobytes * tb = (twobytes *) ar ;
   register unsigned char tt ;

   for( ii=0 ; ii < n ; ii++ ){
      tt       = tb[ii].a ;
      tb[ii].a = tb[ii].b ;
      tb[ii].b = tt ;
   }
}

typedef struct { unsigned char a,b,c,d ; } fourbytes ;

void swap_fourbytes( int n , void * ar )
{
   register int ii ;
   register fourbytes * tb = (fourbytes *) ar ;
   register unsigned char tt ;

   for( ii=0 ; ii < n ; ii++ ){
      tt       = tb[ii].a ;
      tb[ii].a = tb[ii].d ;
      tb[ii].d = tt ;
      tt       = tb[ii].b ;
      tb[ii].b = tb[ii].c ;
      tb[ii].c = tt ;
   }
}
