/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiri <adiri@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 10:47:49 by adiri             #+#    #+#             */
/*   Updated: 2024/12/30 09:43:25 by adiri            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

#define HEIGHT 1080
#define WIDTH 1920
#define BUFFER 200
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <X11/X.h>
#include "minilibx-linux/mlx.h"
#include "get_next_line.h"

typedef struct s_p
{
    int     z;
    int     x;
    int     y;
    int		c;
	double	xx;
	double	yy;
} t_p;

typedef struct s_data
{
    void    *mlx;
    void    *wid;
    int     len_line;
    int     c_line;
    char    *filename;
    int     case_size;
    int     z_size;
	char	*img;
    t_p ***p;
} t_data;

//------------- utils functions -------------------
long    ft_atoi(const char *nptr);
char	**ft_split(char const *s, char c);
void    *ft_calloc(size_t nmemb, size_t size);
void    *ft_memset(void *s, int c, size_t n);
void	ft_error(char *str);
int		ft_count_line(t_data *data);
int		ft_count(char *str, int c);
int		ft_is_hex(char c);
int		ft_get_index(char c);
int		ft_count_par(char *str);
int		ft_get_hex(char *s);
int		ft_get_color(char *str);

//------------- free functions --------------------
void    ft_free_points(t_data *data);
void    ft_f_this(t_data *data, t_p ***p, int n, char **sp);
void	ft_free_arr(char **arr);

//------------- full data --------------------------
void    ft_full_point(t_data *data, char **arr);
void    ft_full_x(t_data *data, t_p ***x, char **arr, int z);
void	ft_is_clear(t_data *data, char **arr);
char	**ft_full_array(t_data *data, int c_line);
void	ft_full_data(t_data *data, char **av, int ac);

#endif
