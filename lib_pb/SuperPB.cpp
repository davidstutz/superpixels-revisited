#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <Elimination/Elimination.h>
#include <MaxFlow/QPBO_MaxFlow.h>

struct scost
{
	float h00;
	float h01;
	float h10;
	float h11;
	float v00;
	float v01;
	float v10;
	float v11;
};

int myfloor(float t)
{
	return int(t)>t? int(t)-1:int(t);
}

template<class T>
void smoothcost(vil_image_view <T> & im, int i1, int i2, int j1, int j2, int hsize, int vsize, float sigma, scost *sc)
{
	float temp;
	float colordiff=0;

	temp = im(i1,j1,0);
	temp-= im(i2,j2,0);
	colordiff += fabs(temp);
	
	temp = im(i1,j1,1);
	temp-= im(i2,j2,1);
	colordiff += fabs(temp);

	temp = im(i1,j1,2);	
	temp-= im(i2,j2,2);
	colordiff += fabs(temp);

	colordiff = exp(-colordiff/sigma);
	

	sc->h01 = colordiff;
	sc->h10 = colordiff;
	sc->v01 = colordiff;
	sc->v10 = colordiff;



	if(myfloor(float(i1)/hsize)!=myfloor(float(i2)/hsize))
	{
		sc->h00 = colordiff;
	}
	else
	{
		sc->h00 = 0;
	}

	if(myfloor(float(i1-hsize/2)/hsize)!=myfloor(float(i2-hsize/2)/hsize))
	{
		sc->h11 = colordiff;
	}
	else
	{
		sc->h11 = 0;
	}

	
	if(myfloor(float(j1)/vsize)!=myfloor(float(j2)/vsize))
	{
		sc->v00 = colordiff;
	}
	else
	{
		sc->v00 = 0;
	}

	if(myfloor(float(j1-vsize/2)/vsize)!=myfloor(float(j2-vsize/2)/vsize))
	{
		sc->v11 = colordiff;
	}
	else
	{
		sc->v11 = 0;
	}

	

	return;
}

//exe inputimage hsize vsize sigma algorithm(0/1)
int main(int argc, char* argv[])
{
	vil_image_view< vxl_byte >  im = vil_load(argv[1]);

	int width = im.ni();
	int height= im.nj();
	int hsize = atoi(argv[2]);
	int vsize = atoi(argv[3]);
	float sigma = atof(argv[4]);
	int algo = atoi(argv[5]);

	Matrix <float> U1(height,width),U2(height,width);//to hold data cost
	Matrix <float> Bh1(height,width),Bh2(height,width);//horizontal smooth cost
	Matrix <float> Bv1(height,width),Bv2(height,width);//vertical smooth cost
	Matrix< unsigned char > solution1(height,width),solution2(height,width);

	struct scost *sc = (struct scost*)malloc(sizeof(struct scost));

	U1.fill(0);
	Bh1.fill(0);
	Bv1.fill(0);
	U2.fill(0);
	Bh2.fill(0);
	Bv2.fill(0);

	for(int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			if(i<width-1)// so it has a horizontal edge to its right neighbor
			{
				smoothcost(im, i, i+1, j, j, hsize, vsize, sigma, sc);
				Bh1[j][i]   = sc->h00-sc->h01-sc->h10+sc->h11;
				U1[j][i]   += sc->h10-sc->h00;
				U1[j][i+1] += sc->h01-sc->h00;
				Bh2[j][i]   = sc->v00-sc->v01-sc->v10+sc->v11;
				U2[j][i]   += sc->v10-sc->v00;
				U2[j][i+1] += sc->v01-sc->v00;
			}

			if(j<height-1)// so it has a vertical edge to its under neighbor
			{
				smoothcost(im, i, i, j, j+1, hsize, vsize, sigma, sc);
				
				Bv1[j][i]   = sc->h00-sc->h01-sc->h10+sc->h11;
				U1[j][i]   += sc->h10-sc->h00;
				U1[j+1][i] += sc->h01-sc->h00;
				Bv2[j][i]   = sc->v00-sc->v01-sc->v10+sc->v11;
				U2[j][i]   += sc->v10-sc->v00;
				U2[j+1][i] += sc->v01-sc->v00;
			}
		}
	}

	if(algo==0)
	{
		MaxFlowQPBO solve1(U1,Bh1,Bv1,solution1);
		MaxFlowQPBO solve2(U2,Bh2,Bv2,solution2);
	}
	else
	{
		Elimination< float >::solve(U1, Bh1, Bv1, solution1);
		Elimination< float >::solve(U2, Bh2, Bv2, solution2);
	}
	
	vil_image_view< int > imh(width,height);
	vil_image_view< int > imv(width,height);

	for(int i=0;i<width;i++)
	{
		for(int j=0;j<height;j++)
		{
			if(solution1[j][i]==0)
				imh(i,j) = myfloor(float(i)/hsize)*2;
			else
				imh(i,j) = myfloor(float(i+hsize/2)/hsize)*2+1;

			if(solution2[j][i]==0)
				imv(i,j) = myfloor(float(j)/vsize)*2;
			else
				imv(i,j) = myfloor(float(j+vsize/2)/vsize)*2+1;
		}
	}

	for(int i=0;i<width-1;i++)
	{
		for(int j=0;j<height-1;j++)
		{
			if(imv(i,j)!=imv(i+1,j)||imh(i,j)!=imh(i+1,j))
			{
				im(i,j,0) = 255;
				im(i,j,1) = 0;
				im(i,j,2) = 0;
				im(i+1,j,0) = 255;
				im(i+1,j,1) = 0;
				im(i+1,j,2) = 0;
			}
			if(imv(i,j)!=imv(i,j+1)||imh(i,j)!=imh(i,j+1))
			{
				im(i,j,0) = 255;
				im(i,j,1) = 0;
				im(i,j,2) = 0;
				im(i,j+1,0) = 255;
				im(i,j+1,1) = 0;
				im(i,j+1,2) = 0;
			}
		}
	}

	vil_save(im,"edge.png");

	return 0;
}

