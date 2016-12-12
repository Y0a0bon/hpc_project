#include <string>
extern double *hFil, *uFil, *vFil, *hPhy, *uPhy, *vPhy;
extern double *hFilg, *uFilg, *vFilg, *hPhyg, *uPhyg, *vPhyg;
extern int size_x, size_y, nb_steps;
extern double dx, dy, dt, pcor, grav, dissip, hmoy, alpha, height, epsilon;
extern bool file_export;
extern std::string export_path;
extern int rang, n, proc_per_line;


#define MASTER 0

#define HFIL(t, i, j) hFil[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * (proc_per_line+2) * size_y ]
#define UFIL(t, i, j) uFil[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * (proc_per_line+2) * size_y ]
#define VFIL(t, i, j) vFil[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * (proc_per_line+2) * size_y ]
#define HPHY(t, i, j) hPhy[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * (proc_per_line+2) * size_y ]
#define UPHY(t, i, j) uPhy[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * (proc_per_line+2) * size_y ]
#define VPHY(t, i, j) vPhy[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * (proc_per_line+2) * size_y ]



#define HFILg(t, i, j) hFilg[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * size_x * size_y ]
#define UFILg(t, i, j) uFilg[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * size_x * size_y ]
#define VFILg(t, i, j) vFilg[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * size_x * size_y ]
#define HPHYg(t, i, j) hPhyg[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * size_x * size_y ]
#define UPHYg(t, i, j) uPhyg[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * size_x * size_y ]
#define VPHYg(t, i, j) vPhyg[ (j) +			\
			    (i) * size_y +		\
			    ((t)%2) * size_x * size_y ]

