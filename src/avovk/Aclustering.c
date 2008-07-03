#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "mrilib.h"
#include "cluster.h"


// from command.c file

static void display_help(void)
{ printf ("Clustering 4segmentation, command-line version.\n");
  printf ("USAGE: cluster [options]\n");
  printf ("options:\n");
  printf ("  -v, --version Version information\n");
  printf ("  -f filename   File loading\n");
  printf ("  -cg a|m       Specifies whether to center each row\n"
          "                in the data\n"
          "                a: Subtract the mean of each row\n"
          "                m: Subtract the median of each row\n"
          "                (default is no centering)\n");
  printf ("  -ng           Specifies to normalize each row in the data\n"
          "                (default is no normalization)\n");
  printf ("  -ca a|m       Specifies whether to center each column \n"
          "                in the data\n"
          "                a: Subtract the mean of each column\n"
          "                m: Subtract the median of each column\n"
          "                (default is no centering)\n");
  printf ("  -na           Specifies to normalize each column in the data\n"
          "                (default is no normalization)\n");
  printf ("  -u jobname    Allows you to specify a different name for the \n"
          "                output files.\n"
          "                (default is derived from the input file name)\n");
  printf ("  -g [0..8]     Specifies the distance measure for gene clustering \n"
         );
  printf ("                0: No gene clustering\n"
          "                1: Uncentered correlation\n"
          "                2: Pearson correlation\n"
          "                3: Uncentered correlation, absolute value\n"
          "                4: Pearson correlation, absolute value\n"
          "                5: Spearman's rank correlation\n"
          "                6: Kendall's tau\n"
          "                7: Euclidean distance\n"
          "                8: City-block distance\n"
          "                (default: 1)\n");
  printf ("  -k number     Specifies whether to run k-means clustering\n"
          "                instead of hierarchical clustering, and the number\n"
          "                of clusters k to use\n");
  printf ("  -r number     For k-means clustering, the number of times the\n"
          "                k-means clustering algorithm is run\n"
          "                (default: 1)\n");
  return;
}


static void display_version(void)
{ printf ("\n"
"Clustering for segmentation, command line version ,\n"
"using the C Clustering Library version 1.38.\n"
"\n"
"Cluster was originally written by Michael Eisen (eisen 'AT' rana.lbl.gov)\n"
"Copyright 1998-99 Stanford University.\n");
  printf ("\n"
"The command line version of Clustering version was created by Andrej Vovk\n"
"and will be surely corrected by Ziad Saad ;),\n"
"\n");
  return;
}

static char* setjobname(const char* basename, int strip)
{ char* jobname;
  int n = strlen(basename);
  if (strip)
  { char* extension = strrchr(basename, '.');
    if (extension) n -= strlen(extension);
  }
  jobname = malloc((n+1)*sizeof(char));
  strncpy (jobname, basename, n);
  jobname[n] = '\0';
  return jobname;
}

static int readnumber(const char word[])
{ char* error = 0;
  long value = strtol(word,&error,0);
  if (*error=='\0') return (int)value;
  else return -1;
}

static char getmetric(int i)
{ switch (i)
  { case 1: return 'u';
    case 2: return 'c';
    case 3: return 'x';
    case 4: return 'a';
    case 5: return 's';
    case 6: return 'k';
    case 7: return 'e';
    case 8: return 'b';
    default: return '\0';
  }
  /* Never get here */
  return '\0';
}



void example_kmeans( int nrows, int ncols, 
                     double** data, 
                     int nclusters, int npass, 
                     char dist, char* jobname)
/* Perform k-means clustering on genes */
{ 
   int i, j, ii, nl, nc;
   //const int nclusters = 3;
   const int transpose = 0;
   //const char dist = 'e';
   const char method = 'a';
   /* For method=='a', the centroid is defined as the mean over all elements
     belonging to a cluster for each dimension.
     For method=='m', the centroid is defined as the median over all elements
     belonging to a cluster for each dimension.
   */

   //int npass = 1;
   int ifound = 0;
   int test=0;
   double error;
   double distance;
   int** index;
   int* count;
   double* weight = malloc(ncols*sizeof(double));
   int* clusterid = malloc(nrows*sizeof(int));
   double** cdata = malloc(nclusters*sizeof(double*));
   int** cmask = malloc(nclusters*sizeof(int*));
   int** mask = NULL;
   int n=0;
   char* filename;
   char* filename2;
   char* filename3;
   FILE *out1=NULL;
   FILE *out2=NULL;
   FILE *out3=NULL;
   
   for (i = 0; i < nclusters; i++)
   { cdata[i] = malloc(ncols*sizeof(double));
    cmask[i] = malloc(ncols*sizeof(int));
   }
   for (i = 0; i < ncols; i++) weight[i] = 1.0;

   mask = (int **)calloc(sizeof(int*), nrows);
   for (ii=0;ii<nrows;++ii) {
    mask[ii] = (int *)calloc(sizeof(int),ncols);
   }

   for (nl=0; nl<nrows; ++nl) {
    for (nc=0; nc<ncols; ++nc) {
      mask[nl][nc] = 1;
    }
   }



   n = 1 + strlen(jobname) + strlen("_K_G") + strlen(".ext");

   if (dist)
   { int dummy = nclusters;
    do n++; while (dummy/=10);
   }
    
   //avovk 
   printf("a je u omari :) \n");
   filename = malloc(n*sizeof(char));
   filename2 = malloc(n*sizeof(char));
   filename3 = malloc(n*sizeof(char));
   sprintf (filename, "%s_K_G%d.kgg", jobname, nclusters);
   out1 = fopen( filename, "w" );
   sprintf (filename2, "%s_K_G%d.dis", jobname, nclusters);
   out2 = fopen( filename2, "w" );
   sprintf (filename3, "%s_K_G%d.cen", jobname, nclusters);
   out3 = fopen( filename3, "w" );

   printf("======================== k-means clustering"
         " ========================\n");

   printf ("\n");
   printf ("----- doing %d passes... go stretch your legs...\n",npass);
   //npass = 3;
   kcluster(nclusters,nrows,ncols,data,mask,weight,transpose,npass,method,dist, 
    clusterid, &error, &ifound);
   printf ("Solution found %d times; ", ifound);
   printf ("within-cluster sum of distances is %f\n", error);
   printf ("------- writing Cluster assignments to file:\t\t"
          " %s_K_G%d.kgg\n",jobname, nclusters);
   for (i = 0; i < nrows; i++)
    fprintf (out1, "%09d\t %d\n", i, clusterid[i]);
   fclose(out1); out1=NULL;

   printf ("------- writing Distance between clusters to file:\t "
          "%s_K_G%d.dis \n", jobname, nclusters);
   fprintf (out2,"------- Distance between clusters:\n");
   index = malloc(nclusters*sizeof(int*));
   count = malloc(nclusters*sizeof(int));
   for (i = 0; i < nclusters; i++) count[i] = 0;
   for (i = 0; i < nrows; i++) count[clusterid[i]]++;
   for (i = 0; i < nclusters; i++) index[i] = malloc(count[i]*sizeof(int));
   for (i = 0; i < nclusters; i++) count[i] = 0;
   for (i = 0; i < nrows; i++){ 
      int id = clusterid[i];
      index[id][count[id]] = i;
      count[id]++;
   }  
   distance =
    clusterdistance(nrows, ncols, data, mask, weight, count[0], count[1],
		    index[0], index[1], 'e', 'a', 0); 
   fprintf(out2,"Distance between 0 and 1: %7.3f\n", distance);
   distance =
    clusterdistance(nrows, ncols, data, mask, weight, count[0], count[2],
		    index[0], index[2], 'e', 'a', 0); 
   fprintf(out2,"Distance between 0 and 2: %7.3f\n", distance);
   distance =
    clusterdistance(nrows, ncols, data, mask, weight, count[1], count[2],
		    index[1], index[2], 'e', 'a', 0); 
   fprintf(out2,"Distance between 1 and 2: %7.3f\n", distance);
   fclose(out2); out2=NULL;


   printf ("------- writing Cluster centroids to file:\t\t"
          "%s_K_G%d.cen\n",jobname, nclusters);
   fprintf (out3,"------- Cluster centroids:\n");
   getclustercentroids(nclusters, nrows, ncols, data, mask, clusterid,
                      cdata, cmask, 0, 'a');
   fprintf(out3,"   coefficients:");
   for(i=0; i<ncols; i++) fprintf(out3,"\t%7d", i);
   fprintf(out3,"\n");
   for (i = 0; i < nclusters; i++){ 
      fprintf(out3,"Cluster %2d:", i);
      for (j = 0; j < ncols; j++) fprintf(out3,"\t%7.3f", cdata[i][j]);
      fprintf(out3,"\n");
   }
   fclose(out3); out3=NULL;
   printf("Done...\n");
   for (i = 0; i < nclusters; i++) free(index[i]);
   free(index);
   free(count);

   for (i = 0; i < nclusters; i++){ 
      free(cdata[i]);
      free(cmask[i]);
   }
   for (ii=0;ii<nrows;++ii) {
      if (mask[ii]) free(mask[ii]);
   }
   
   free(cdata);
   free(cmask);
   free(clusterid);
   free(weight);
   free(mask);
   return;
}


/* ========================================================================= */

int main(int argc, char **argv)
{ 
   int ii=0, ncol=0, nrow=0, nl=0, nc=0, posi=0, posj=0, posk=0;
   //int nclust=atoi(argv[2]);
   MRI_IMAGE *im = NULL;
   double *dar = NULL;
   double **D = NULL;
   //int **mask = NULL;

   //from command.c

   int i = 1;
   char* filename = 0;
   char* jobname = 0;
   int l = 0;
   int k = 4;
   int r = 1;
   int s = 0;
   int x = 2;
   int y = 1;
   int Rows, Columns;
   char distmetric = 'u';
   char arraymetric = '\0';
   char method = 'm';
   char cg = '\0';
   char ca = '\0';
   int ng = 0;
   int na = 0;
   while (i < argc)
   { const char* const argument = argv[i];
    i++;
    if (strlen(argument)<2)
    { printf("ERROR: missing argument\n");
      return 0;
    }
    if (argument[0]!='-')
    { printf("ERROR: unknown argument\n");
      return 0;
    }
    if(!strcmp(argument,"--version") || !strcmp(argument,"-v"))
    { display_version();
      return 0;
    }
    if(!strcmp(argument,"--help") || !strcmp(argument,"-h"))
    { display_help();
      return 0;
    }
    if(!strcmp(argument,"-cg"))
    { if (i==argc || strlen(argv[i])>1 || !strchr("am",argv[i][0]))
      { printf ("Error reading command line argument cg\n");
        return 0;
      }
      cg = argv[i][0];
      i++;
      continue;
    }
    if(!strcmp(argument,"-ca"))
    { if (i==argc || strlen(argv[i])>1 || !strchr("am",argv[i][0]))
      { printf ("Error reading command line argument ca\n");
        return 0;
      }
      ca = argv[i][0];
      i++;
      continue;
    }
    if(!strcmp(argument,"-ng"))
    { ng = 1;
      continue;
    }
    if(!strcmp(argument,"-na"))
    { na = 1;
      continue;
    }
    switch (argument[1])
    { case 'l': l=1; break;
      case 'u':
      { if (i==argc)
        { printf ("Error reading command line argument u: "
                  "no job name specified\n");
          return 0;
        }
        jobname = setjobname(argv[i],0);
        i++;
        break;
      }
      case 'f':
      { if (i==argc)
        { printf ("Error reading command line argument f: "
                  "no file name specified\n");
          return 0;
        }
        filename = argv[i];
        i++;
        break;
      }
      case 'g':
      { int g;
        if (i==argc)
        { printf ("Error reading command line argument g: parameter missing\n");
          return 0;
        }
        g = readnumber(argv[i]);
        if (g < 0 || g > 9)
        { printf ("Error reading command line argument g: "
                  "should be between 0 and 9 inclusive\n");
          return 0;
        }
        i++;
        distmetric = getmetric(g);
        break;
      }

      case 'k':
      { if (i==argc)
        { printf ("Error reading command line argument k: "
                  "parameter missing\n");
          return 0;
        }
        k = readnumber(argv[i]);
        if (k < 1)
        { printf ("Error reading command line argument k: "
                  "a positive integer is required\n");
          return 0;
        }
        i++;
        break;
      }
      case 'r':
      { if (i==argc)
        { printf ("Error reading command line argument r: parameter missing\n");
          return 0;
        }
        r = readnumber(argv[i]);
        if (r < 1)
        { printf ("Error reading command line argument r: "
                  "a positive integer is required\n");
          return 0;
        }
        i++;
        break;
      }
      default: printf ("Unknown option\n");
    }
   }

   if(jobname==0) jobname = setjobname(filename,1);

   /*  else
   { display_help();
    return 0;
   }
   */

    if (argc < 2) {
      display_help();
      return 0;
    }

   //printf("num of clusters %d \n",nclust);
   fprintf(stderr,"Patience, reading %s...\n ", filename);
   im = mri_read_double_1D (filename);
   /* ZIAD I get this warning
   Aclustering.c:408: warning: passing argument 1 of mri_read_double_1D 
                           discards qualifiers from pointer target type 
   Andrej: filename was declared as (const char *), 
          but the function expects (char *)              */
   if (!im) {
    fprintf(stderr,"Error: Failed to read matrix data from %s\n",
	    argv[2]);
    return(-1);
   }
   ncol = im->ny;
   nrow = im->nx;
   fprintf (stderr,"Have %d cols, %d rows\nNow...\n", ncol, nrow);

   /* now just get the array and kill the rest */
   dar = MRI_DOUBLE_PTR(im);

   /* make sure that pointer is set to NULL in im, or risk hell */
   mri_clear_data_pointer(im);
   if (im) mri_free(im); im = NULL; /* now kill im */


   /* for double loop*/
   D = (double **)calloc(sizeof(double*), nrow-1);
   //mask = (int **)calloc(sizeof(int*), nrow-1);
   for (ii=0;ii<(nrow-1);++ii) {
    D[ii] = (double *)calloc(sizeof(double), ncol-1);
    //mask[ii] = (int *)calloc(sizeof(int),ncol-1);
   }

   for (nl=1; nl<nrow; ++nl) {
    for (nc=1; nc<ncol; ++nc) {
      D[nl-1][nc-1] = dar[nl+nc*nrow];
      //mask[nl-1][nc-1] = 1;
      //fprintf(stdout,"%g ",D[nl-1][nc-1]);
    }
    //fprintf(stdout,"\n");
   }

   //show_data(nrows, ncols, data, mask);
   //example_mean_median(nrows, ncols, data, mask);
   //distmatrix = example_distance_gene(nrows, ncols, data, mask);
   //if (distmatrix) example_hierarchical(nrows, ncols, data, mask, distmatrix);
   //example_distance_array(nrows, ncols, data, mask);

   example_kmeans(nrow-1, ncol-1, D, k, r, distmetric, jobname);
   //example_som(nrows, ncols, data, mask);


   free(dar); dar = NULL; /* done with input array */
   // To free D 
   for (ii=0;ii<(nrow-1);++ii) {
    if (D[ii]) free(D[ii]);
    // if (mask[ii]) free(mask[ii]);
   }
   free(D);
   //free(mask);
   free(jobname);
   //free();

   fprintf (stderr,"\n");
   return 0;
   }

   /* How to compile:
   I copied Aclustering.c and cluster.c to /usr/local/bin/afni/src
   then compile with:
   gcc -Wall -Wno-unused Aclustering2.c cluster.c -o Aclustering -Inifti/niftilib -Inifti/nifticdf -Inifti/znzlib -L. -bind_at_load -l3DEdge -lmri -lf2c -lmri /usr/lib64/libXm.a -lXm -lXmu -lXp -lXpm -lXext -lXt -lX11 -lz -lexpat -lm -lc

   The output are 3 files
   - cluster_out.kgg -- as before, index number and cluster number
   - cluster_out.dis -- 
   - cluster_out.cen -- 


   actually
   cp /home/drejc/c_temp/Aclustering.c ./

   gcc -Wall -g -Wno-unused Aclustering.c cluster.c -o Aclustering -Inifti/niftilib -Inifti/nifticdf -Inifti/znzlib -L. -l3DEdge -lmri -lf2c -lmri /usr/lib64/libXm.a -lXm -lXmu -lXp -lXpm -lXext -lXt -lX11 -lz -lexpat -lm -lc

   cp Aclustering /home/drejc/segm4ziad/toandrej/out_mprage_10_smpl/

   */
