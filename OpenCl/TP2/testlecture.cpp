  FILE *fp = fopen("data/grd_618360_6754408_2.txt","r");
  if (fp==NULL){
    exit(0);
  }
  

  
  fscanf(fp,"%d",&nx);
  fscanf(fp,"%d",&ny);
  fscanf(fp,"%d",&gauche);
  fscanf(fp,"%d",&droit);
  fscanf(fp,"%d",&cell);
  fscanf(fp,"%d",&nodata);

  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      fscanf(fp,"%f",&hauteur);
      tab[i*nx+j] = hauteur;
    }
  }
  fclose(fp);
