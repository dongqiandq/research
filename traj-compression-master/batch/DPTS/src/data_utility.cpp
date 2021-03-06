/*
 *	Author: Cheng Long
 *	Email: clong@cse.ust.hk
 */



#include "data_utility.h"


#ifndef WIN32
/*
* get_curtime gets the current rusage info.
*
* @cur_t is used for storing the rusage information.
*/
void get_cur_time( rusage* cur_t)
{
	if( getrusage( RUSAGE_SELF, cur_t) != 0)
	{
		fprintf( stderr, "The invokation of getrusage fails.\n");
		exit( 0);
	}
}

/*
* GetTime calculates the time information.
* 
* @sta_t and end_t indicate the rusage information at the start and end, respectively.
* @user_t and system_t are used for storing the time information.
*
*/
void collect_time( struct rusage* sta_t, struct rusage* end_t, float* user_t)	//, float* system_t)
{
	(*user_t) = ((float)(end_t->ru_utime.tv_sec - sta_t->ru_utime.tv_sec)) + 
		((float)(end_t->ru_utime.tv_usec - sta_t->ru_utime.tv_usec)) * 1e-6;
/*
	(*system_t) = ((float)(end_t->ru_stime.tv_sec - sta_t->ru_stime.tv_sec)) +
		((float)(end_t->ru_stime.tv_usec - sta_t->ru_stime.tv_usec)) * 1e-6;
*/
}

#endif

/*
 *	Preprocess the real data obtaine at www.dis.uniroma1.it/~challenge9/download.shtml.
 */
void real_data_preprocess( )
{
	int cnt, id1, id2, edge_cnt, edge_tag;
	float dist, coord1, coord2;
	char tmp[ MAX_LINE_LENG];
	char line_tmp[ MAX_LINE_LENG];
	d_config_t* d_config_v;
	FILE* c_fp, *i_fp, *o_fp;

	if( ( c_fp = fopen( REAL_DATA_CONFIG, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open %s\n", REAL_DATA_CONFIG);
		exit( 0);
	}

	d_config_v = ( d_config_t*)malloc( sizeof( d_config_t));

	cnt = 1;
	while( fscanf( c_fp, "%s%s%i", d_config_v->input_f, d_config_v->output_f, &d_config_v->file_opt) != EOF)
	{
		if( ( i_fp = fopen( d_config_v->input_f, "r")) == NULL ||
			( o_fp = fopen( d_config_v->output_f, "w")) == NULL)
		{
			fprintf( stderr, "Cannot open %s or %s\n", d_config_v->input_f, d_config_v->output_f);
			exit( 0);
		}

		printf( "Dealing with #instance %i...\n", cnt++);
		
		//Filter out the comment information.
		fscanf( i_fp, "%s", tmp);
		while( strcmp( tmp, "v") != 0 && strcmp( tmp, "a") != 0)
		{
			fgets( line_tmp, MAX_LINE_LENG, i_fp);
			fscanf( i_fp, "%s", tmp);
		}

		//Read & Write the data information.
		edge_cnt = 0;
		edge_tag = 1;
		while( true)
		{
			if( d_config_v->file_opt == 2)
			{
				//Dealing with the edge file.
				fscanf( i_fp, "%i%i%f", &id1, &id2, &dist);
				if( edge_tag == 1)
				{
					fprintf( o_fp, "%i\t%i\t%i\t%0.1f\n", edge_cnt, id1-1, id2-1, dist);
					edge_cnt ++;
					edge_tag = 0;
				}
				else
					edge_tag = 1;
			}
			else	//tmp == 'v'
			{
				//Dealing with the node file.
				fscanf( i_fp, "%i %f %f", &id1, &coord1, &coord2);
				fprintf( o_fp, "%i\t%f\t%f\n", id1-1, coord1, coord2);
			}
			//fgets( line_tmp, MAX_LINE_LENG, i_fp);
			//fputs( line_tmp, o_fp);

			if( fscanf( i_fp, "%s", tmp) == EOF)
				break;
		}

		fclose( i_fp);
		fclose( o_fp);
	}

	fclose( c_fp);
}


/*
 *	Read the config information.
 */
config_t* read_config( )
{
	config_t* config_v;
	FILE* c_fp;

	config_v = ( config_t*)malloc( sizeof( config_t));

	if( ( c_fp = fopen( CONFIG_FILE, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open %s\n", CONFIG_FILE);
		exit( 0);
	}

	fscanf( c_fp, "%s%s", config_v->vertex_f, config_v->edge_f);
	fscanf( c_fp, "%i%i", &( config_v->vertex_n), &( config_v->edge_n));

	fscanf( c_fp, "%f%i%f", &( config_v->eps), &( config_v->L), &( config_v->L_percent));
	fscanf( c_fp, "%i%i", &( config_v->pro_opt), &( config_v->alg_opt));

	fscanf( c_fp, "%i", &( config_v->q_times));

	fclose( c_fp);

	return config_v;
}

/*
 *	Allocate the memory for the RN_graph_t structure.
 *
 *	@vertex_n indicates the number of vertices.
 *
 *	@return a (blank) RN_graph_t structure.
 */
RN_graph_t* RN_graph_alloc( int vertex_n)
{
	int i;
	RN_graph_t* RN_graph_v;
	RN_graph_list_t* g_list_v;

	RN_graph_v = ( RN_graph_t*)malloc( sizeof( RN_graph_t));
	memset( RN_graph_v, 0, sizeof( RN_graph_t));

	RN_graph_v->vertex_n = vertex_n;
	RN_graph_v->head_v = ( RN_graph_head_t*)malloc( sizeof( RN_graph_head_t) * vertex_n);
	memset( RN_graph_v->head_v, 0, sizeof( RN_graph_head_t) * vertex_n);

	for( i=0; i<vertex_n; i++)
	{
		g_list_v = ( RN_graph_list_t*)malloc( sizeof( RN_graph_list_t));
		memset( g_list_v, 0, sizeof( RN_graph_list_t));
		
		RN_graph_v->head_v[ i].list = g_list_v;
	}

	/*s*/
	RN_graph_v->m_size += sizeof( RN_graph_t) + 
						sizeof( RN_graph_head_t) * vertex_n +
						sizeof( RN_graph_list_t) * vertex_n;
	/*s*/

	return RN_graph_v;
}

/*
 *	Release a RN_graph_t structure.
 */
void RN_graph_release( RN_graph_t* RN_graph_v)
{
	int i;
	RN_graph_list_t* g_list_v1, *g_list_v2;

	/*s*/
	stat_v.memory_v -= RN_graph_v->m_size;
	/*s*/

	for( i=0; i<RN_graph_v->vertex_n; i++)
	{
		g_list_v1 = RN_graph_v->head_v[ i].list;
		g_list_v2 = g_list_v1->next;
		while( g_list_v2 != NULL)
		{
			free( g_list_v1);
			g_list_v1 = g_list_v2;
			g_list_v2 = g_list_v1->next;
		}
		free( g_list_v1);
	}
	
	free( RN_graph_v->head_v);
	free( RN_graph_v);
}

/*
 *	Calculate the absolute slope information of a vector.
 *
 *	@(loc_v1, loc_v2) indicates the vector.
 *
 *	@return the absolute slope information.
 */
float calc_abs_slope( loc_t* loc_v1, loc_t* loc_v2)
{
	float slope;
	double cos_v;
	double leng, dot_product;

	leng = sqrt( pow( loc_v2->x - loc_v1->x, 2) + 
					pow( loc_v2->y - loc_v1->y, 2));

	//
	//if( leng >= -1e-12 && leng <= 1e12)
	//	return 0;
	//
	
	//dot_product of the vector and (1, 0).
	dot_product = loc_v2->x - loc_v1->x;

	cos_v = dot_product / leng;

	slope = acos( cos_v);

	if( loc_v2->y < loc_v1->y)
		slope = 2 * PI - slope;
	
	return slope;
}

/*
 *	Read the data (road network).
 */
RN_graph_t* read_data( config_t* config_v)
{
	int id_v, vertex_n, cnt, ord, id1, id2;
	float dist;
	RN_graph_t* RN_graph_v;
	RN_graph_list_t* g_list_v;
	FILE* i_vertex_fp, *i_edge_fp;

	//Allocate the memory.
	vertex_n = config_v->vertex_n;
	RN_graph_v = RN_graph_alloc( vertex_n);
	RN_graph_v->edge_n = config_v->edge_n;

	if( ( i_vertex_fp = fopen( config_v->vertex_f, "r")) == NULL ||
		( i_edge_fp = fopen( config_v->edge_f, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open %s or %s\n", config_v->vertex_f, config_v->edge_f);
		exit( 0);
	}

	//Read the vertex (coordinate) information.
	cnt = 0;
	while( fscanf( i_vertex_fp, "%i", &id_v) != EOF)
	{
		fscanf( i_vertex_fp, "%f%f", &( RN_graph_v->head_v[ cnt].loc_v.x),
									&( RN_graph_v->head_v[ cnt].loc_v.y));
		cnt ++;
	}

	fclose( i_vertex_fp);
	
	//Read the edge (road segment) information.
	cnt = 0;
	while( fscanf( i_edge_fp, "%i%i%i%f", &ord, &id1, &id2, &dist) != EOF)
	{
		//id1 --;
		//id2 --;
		
		//edge: id1->id2.
		g_list_v = ( RN_graph_list_t*)malloc( sizeof( RN_graph_list_t));
		memset( g_list_v, 0, sizeof( RN_graph_list_t));
		g_list_v->id = id2;
		g_list_v->leng = dist;
		//g_list_v->slope = calc_abs_slope( &RN_graph_v->head_v[ id1].loc_v, &RN_graph_v->head_v[ id2].loc_v);

		RN_graph_v->head_v[ id1].degree ++;
		g_list_v->next = RN_graph_v->head_v[ id1].list->next;
		RN_graph_v->head_v[ id1].list->next = g_list_v;		

		/**/
		//edge: id2->id1.
		g_list_v = ( RN_graph_list_t*)malloc( sizeof( RN_graph_list_t));
		memset( g_list_v, 0, sizeof( RN_graph_list_t));
		g_list_v->id = id1;
		g_list_v->leng = dist;
		//g_list_v->slope = RN_graph_v->head_v[ id1].next->next->slope;

		RN_graph_v->head_v[ id2].degree ++;
		g_list_v->next = RN_graph_v->head_v[ id2].list->next;
		RN_graph_v->head_v[ id2].list->next = g_list_v;		
		/**/
		cnt ++;
	}

	RN_graph_v->edge_n = cnt;

	/*s*/
	RN_graph_v->m_size += sizeof( RN_graph_list_t) * 2 * cnt;
	stat_v.memory_v += RN_graph_v->m_size;
	if( stat_v.memory_max < stat_v.memory_v)
		stat_v.memory_max = stat_v.memory_v;
	/*s*/

	/*t/
	if( cnt != RN_graph_v->edge_n)
	{
		fprintf( stderr, "The configuration of edge_n is inconsistent with the # of edges in the file.\n");
		RN_graph_v->edge_n = cnt;
	}
	/*t*/

	fclose( i_edge_fp);

	return RN_graph_v;
}

/*
 *	Collect the statistics of non-passing nodes in the road network.
 */
void collect_non_passing_stat( )
{
	int i, vertex_n, ins_cnt, id1, id2, non_passing_cnt, tmp_i;
	float percent, tmp_f;
	int* degree_cnt;
	char f_name[ MAX_FILENAME_LENG];

	FILE* c_fp, *i_fp, *s_fp;

	if( ( c_fp = fopen( NON_PASSING_CONFIG, "r")) == NULL ||
		( s_fp = fopen( NON_PASSING_STAT, "w")) == NULL)
	{
		fprintf( stderr, "Cannot open %s or %s\n", NON_PASSING_CONFIG, NON_PASSING_STAT);
		exit( 0);
	}

	//Handling the cases.
	ins_cnt = 1;
	while( fscanf( c_fp, "%s%i", f_name, &vertex_n) != EOF)
	{
		printf( "Handling Case %i...\n", ins_cnt ++);

		degree_cnt = ( int*)malloc( vertex_n * sizeof( int));
		memset( degree_cnt, 0, vertex_n * sizeof( int));
	
		if( ( i_fp = fopen( f_name, "r")) == NULL)
		{
			fprintf( stderr, "Cannot open %s\n", f_name);
			continue;
		}
		
		while( fscanf( i_fp, "%i%i%i%f", &tmp_i, &id1, &id2, &tmp_f) != EOF)
		{
			degree_cnt[ id1] ++;
			degree_cnt[ id2] ++;
		}

		//Collect the degree information.
		non_passing_cnt = 0;
		for( i=0; i<vertex_n; i++)
		{
			if( degree_cnt[ i] == 2)
				non_passing_cnt ++;
		}

		percent = ((float)non_passing_cnt) / vertex_n;

		printf( "Percentage of non_passing nodes: %f\n", percent);
		fprintf( s_fp, "%s\t%i\t%i\t%f\n", f_name, vertex_n, non_passing_cnt, percent);

		free( degree_cnt);
	}

	fclose( c_fp);
	fclose( s_fp);
}





//Updated on 27 Jan 2013.
/*
 *	The new version of read_config.
 */
emp_config_t* emp_read_config_v2( )
{
	FILE* c_fp;
	emp_config_t* emp_config_v;

	if( ( c_fp = fopen( CONFIG_FILE, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open the config.txt\n");
		exit( 0);
	}

	emp_config_v = ( emp_config_t*)malloc( sizeof( emp_config_t));
	memset( emp_config_v, 0, sizeof( emp_config_t));

	fscanf( c_fp, "%s%i%i", emp_config_v->f_name, &emp_config_v->pos_n, 
								//&emp_config_v->precision_thr, 
								&emp_config_v->line_ignore_n);

	emp_config_v->precision_thr = 0.000000;
	
	fscanf( c_fp, "%f%i", &emp_config_v->W_percent, &emp_config_v->alg_opt);//, &emp_config_v->appr_b_tag);

	//fscanf( c_fp, "%i", &emp_config_v->dataset_tag);

	emp_config_v->dataset_tag = 1;

	return emp_config_v;
}

emp_config_t* emp_read_config( )
{
	FILE* c_fp;
	emp_config_t* emp_config_v;

	if( ( c_fp = fopen( CONFIG_FILE, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open the config.txt\n");
		exit( 0);
	}

	emp_config_v = ( emp_config_t*)malloc( sizeof( emp_config_t));
	memset( emp_config_v, 0, sizeof( emp_config_t));

	fscanf( c_fp, "%s%i", emp_config_v->f_name, &emp_config_v->pos_n);//, 
								//&emp_config_v->precision_thr, 
								//&emp_config_v->line_ignore_n);

	emp_config_v->precision_thr = 0.000000;
	
	fscanf( c_fp, "%f%i", &emp_config_v->eps, &emp_config_v->alg_opt);//, &emp_config_v->appr_b_tag);

	fscanf( c_fp, "%i", &emp_config_v->dataset_tag);

	return emp_config_v;
}

/*
 *	Allocate a tra_list_t struct.
 */
tra_list_t* tra_list_alloc( )
{
	tra_list_t* tra_list_v;

	tra_list_v = ( tra_list_t*)malloc( sizeof( tra_list_t));
	memset( tra_list_v, 0, sizeof( tra_list_t));

	tra_list_v->head = ( tra_node_t*)malloc( sizeof( tra_node_t));
	memset( tra_list_v->head, 0, sizeof( tra_node_t));

	tra_list_v->head->pos_id = -1;

	tra_list_v->rear = tra_list_v->head;

	/*s*/
	emp_stat_v.memory_v += sizeof( tra_list_t) + sizeof( tra_node_t);
	if( emp_stat_v.memory_v > emp_stat_v.memory_max)
		emp_stat_v.memory_max = emp_stat_v.memory_v;
	/*s*/

	return tra_list_v;
}

/*
 *	Append a tra_node_t entry with @tri_v to a tra_list_t struct @tra_list_v.
 */
void append_tra_list_entry( tra_list_t* tra_list_v, triplet_t tri_v)
{
	tra_node_t* tra_node_v;

	tra_node_v = ( tra_node_t*)malloc( sizeof( tra_node_t));
	memset( tra_node_v, 0, sizeof( tra_node_t));

	tra_node_v->pos_id = tra_list_v->rear->pos_id + 1;
	tra_node_v->tri_v = tri_v;

	tra_list_v->rear->next = tra_node_v;
	tra_list_v->rear = tra_node_v;
	tra_list_v->pos_n ++;

	/*s*/
	emp_stat_v.memory_v += sizeof( tra_node_t);
	if( emp_stat_v.memory_v > emp_stat_v.memory_max)
		emp_stat_v.memory_max = emp_stat_v.memory_v;
	/*s*/

	return;
}

/*
 *	Release a tra_list_t struct.
 */
void tra_list_release( tra_list_t* tra_list_v)
{
	tra_node_t* tra_node_v1, *tra_node_v2;

	tra_node_v1 = tra_list_v->head;
	tra_node_v2 = tra_node_v1->next;
	while( tra_node_v2 != NULL)
	{
		free( tra_node_v1);

		tra_node_v1 = tra_node_v2;
		tra_node_v2 = tra_node_v1->next;
	}
	free( tra_node_v1);

	/*s*/
	emp_stat_v.memory_v -= ( tra_list_v->pos_n + 1) * sizeof( tra_node_t) + sizeof( tra_list_t);
	/*s*/

	free( tra_list_v);
}

/*
 *	The new version of read_data.
 *
 *	Specific to the Geolife data.
 */
tra_list_t*	emp_read_data_v1( emp_config_t* emp_config_v)
{
	int i, j;
	char tmp[ MAX_LINE_LENG];
	char* tok;

	float pre_x, pre_y;

	triplet_t tri_v;
	tra_list_t* tra_list_v;

	FILE* i_fp;

	if( ( i_fp = fopen( emp_config_v->f_name, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open the data file.\n");
		exit( 0);
	}

	//Filter the useless lines if necessary.
	for( i=0; i<emp_config_v->line_ignore_n; i++)
	{
		fgets( tmp, MAX_LINE_LENG, i_fp);
	}

	//Read the positions.
	pre_x = FLT_MAX;
	pre_y = FLT_MAX;
	tra_list_v = tra_list_alloc( );
	for( i=0; i<emp_config_v->pos_n; i++)
	{
		//Read the line.
		fgets( tmp, MAX_LINE_LENG, i_fp);

		//Parse the line to get the x and y coordinates.
		if( !(tok = strtok( tmp, ",")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.loc_v.x = atof( tok);

		//t
		//printf( "x: %f\n", tri_v.loc_v.x);
		//t

		if( !(tok = strtok( NULL, ",")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.loc_v.y = atof( tok);

		//t
		//printf( "y: %f\n", tri_v.loc_v.y);
		//t

		//t
		if( i == 71)
			printf( "");
		//t

		if( fabs( tri_v.loc_v.x - pre_x) <= emp_config_v->precision_thr &&
			fabs( tri_v.loc_v.y - pre_y) <= emp_config_v->precision_thr)
			continue;


		pre_x = tri_v.loc_v.x;
		pre_y = tri_v.loc_v.y;

		//useless information.
		for( j=0; j<3; j++)
		{
			if( !(tok = strtok( NULL, ",")))
			{
				fprintf( stderr, "Data format inconsistency.\n");
				exit( 0);
			}
		}

		//year.
		if( !(tok = strtok( NULL, "-")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.year = atoi( tok);

		//month.
		if( !(tok = strtok( NULL, "-")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.month = atoi( tok);

		//day.
		if( !(tok = strtok( NULL, ",")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.day = atoi( tok);

		//hour.
		if( !(tok = strtok( NULL, ":")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.hour = atoi( tok);

		//minute.
		if( !(tok = strtok( NULL, ":")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.min = atoi( tok);

		//second.
		if( !(tok = strtok( NULL, "")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}
		
		tri_v.sec = atoi( tok);

	
		/*t/
		printf( "%f\t%f\n", tri_v.loc_v.x, tri_v.loc_v.y);
		/*t*/

		append_tra_list_entry( tra_list_v, tri_v);
	}		

	return tra_list_v;
}

/*
 *	Specific to the T-Drive data.
 */
tra_list_t*	emp_read_data_v2( emp_config_t* emp_config_v)
{
	int i;
	char tmp[ MAX_LINE_LENG];
	char* tok;

	float pre_x, pre_y;

	triplet_t tri_v;
	tra_list_t* tra_list_v;

	FILE* i_fp;

	if( ( i_fp = fopen( emp_config_v->f_name, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open the data file.\n");
		exit( 0);
	}

	//Filter the useless lines if necessary.
	for( i=0; i<emp_config_v->line_ignore_n; i++)
	{
		fgets( tmp, MAX_LINE_LENG, i_fp);
	}

	//Read the positions.
	pre_x = FLT_MAX;
	pre_y = FLT_MAX;
	tra_list_v = tra_list_alloc( );
	for( i=0; i<emp_config_v->pos_n; i++)
	{
		//Read the line.
		fgets( tmp, MAX_LINE_LENG, i_fp);

		//Parse the line to get the x and y coordinates.
		//useless.
		if( !(tok = strtok( tmp, ",")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		//year.
		if( !(tok = strtok( NULL, "-")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.year = atoi( tok);

		//month.
		if( !(tok = strtok( NULL, "-")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.month = atoi( tok);

		//day.
		if( !(tok = strtok( NULL, " ")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.day = atoi( tok);

		//hour.
		if( !(tok = strtok( NULL, ":")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.hour = atoi( tok);

		//min.
		if( !(tok = strtok( NULL, ":")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.min = atoi( tok);

		//year.
		if( !(tok = strtok( NULL, ",")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.sec = atoi( tok);

		//x coordinate.
		if( !(tok = strtok( NULL, ",")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.loc_v.x = atof( tok);

		//y coordinate.
		if( !(tok = strtok( NULL, "")))
		{
			fprintf( stderr, "Data format inconsistency.\n");
			exit( 0);
		}

		tri_v.loc_v.y = atof( tok);

		if( fabs( tri_v.loc_v.x - pre_x) <= emp_config_v->precision_thr &&
			fabs( tri_v.loc_v.y - pre_y) <= emp_config_v->precision_thr)
			continue;


		pre_x = tri_v.loc_v.x;
		pre_y = tri_v.loc_v.y;

		//
		//tri_v.t = 0;

		/*t/
		printf( "%f\t%f\n", tri_v.loc_v.x, tri_v.loc_v.y);
		/*t*/

		append_tra_list_entry( tra_list_v, tri_v);
	}		

	return tra_list_v;
}


/*
 *	A sub-procedure of combine_files.
 */
int combine_files_sub( char* dir_path, char** file_names, int file_n, char* output_f, int size_thr)
{
	int i, j, cnt;
	FILE* i_fp, *o_fp;
	char i_file[ MAX_LINE_LENG];
	char tmp[ MAX_LINE_LENG];
	char str_line[ MAX_LINE_LENG];

	if( ( o_fp = fopen( output_f, "w")) == NULL)
	{
		fprintf( stderr, "Cannot open the %s file.\n", output_f);
		exit( 0);
	}
	
	cnt = 0;
	for( i=0; i<file_n; i++)
	{
		strcpy( i_file, dir_path);
		strcat( i_file, "/");
		strcat( i_file, file_names[ i]);

		/*t*/
		printf( "Reading %s ...\n", i_file);
		/*t*/

		if( ( i_fp = fopen( i_file, "r")) == NULL)
		{
			fprintf( stderr, "Cannot open %s.\n", i_file);
			exit( 0);
		}
		
		//Read the file.
		//Filter the useless lines.
		for( j=0; j<NUM_LINES_IGNORE; j++)
			fgets( tmp, MAX_LINE_LENG, i_fp);

		while( fgets( str_line, MAX_LINE_LENG, i_fp))
		{
			fputs( str_line, o_fp);
			cnt ++;

			/**/
			//For preparing the large datasets.
			if( cnt >= size_thr)
			{
				fclose( i_fp);
				fclose( o_fp);
				
				return cnt;
			}
			/**/
		}

		//
		fclose( i_fp);
	}

	fclose( o_fp);
	
	return cnt;
}


/*
 *	Collect the trajectory size statistics.
 */
int collect_size( char* dir_path, char** file_names, int file_n, char* output_f)
{
	int i, j, cnt;
	FILE* i_fp, *o_fp;
	char i_file[ MAX_LINE_LENG];
	char tmp[ MAX_LINE_LENG];
	char str_line[ MAX_LINE_LENG];

	if( ( o_fp = fopen( output_f, "w")) == NULL)
	{
		fprintf( stderr, "Cannot open the %s file.\n", output_f);
		exit( 0);
	}
	
	/*t/
	printf( "Num of files: %i\n", file_n);
	/*t*/
	cnt = 0;
	for( i=0; i<file_n; i++)
	{
		strcpy( i_file, dir_path);
		strcat( i_file, "/");
		strcat( i_file, file_names[ i]);

		/*t*/
		printf( "Reading %s ...\n", i_file);
		/*t*/

		if( ( i_fp = fopen( i_file, "r")) == NULL)
		{
			fprintf( stderr, "Cannot open %s.\n", i_file);
			exit( 0);
		}
		
		//Read the file.
		//Filter the useless lines.
		for( j=0; j<NUM_LINES_IGNORE; j++)
			fgets( tmp, MAX_LINE_LENG, i_fp);

		while( fgets( str_line, MAX_LINE_LENG, i_fp))
		{
			//fputs( str_line, o_fp);
			cnt ++;
		}

		fprintf( o_fp, "%s\t%i\n", file_names[ i], cnt);

		/*t/
		if( cnt >= SIZE_THR)
			printf( "%s\t%s\t%i\n", dir_path, file_names[ i], cnt);
		/*t*/

		//
		fclose( i_fp);
	}

	fclose( o_fp);
	
	return cnt;
}

/*	
 *	For alphabetical sorting.
 */
int	str_cmp( const void* str_1, const void* str_2)
{
	return strcmp( *( char**)str_1, *( char**)str_2);
}


#ifndef WIN32
/*
 *	Combine the files under a given directory.
 */
void combine_files( int func_opt)
{
	int i, cnt, pos_n, ins_n, size_thr, file_cnt, pos_cnt;
	char dir_path[ MAX_FILENAME_LENG];
	char taxi_id[ MAX_FILENAME_LENG];
	char output_f[ MAX_FILENAME_LENG];
	//char str_tmp[ MAX_FILENAME_LENG];
	char file_loc[ MAX_FILENAME_LENG];
	char** file_names;

	FILE* c_fp, *o_fp, *i_fp, *s_fp;
	
	DIR* dir_p;
	struct dirent* dir_r;

	unsigned char is_file =0x8;
	
	strcpy( file_loc, "./data/");

	if( ( c_fp = fopen( COMBINE_FILE_CONFIG, "r")) == NULL)
	{
		fprintf( stderr, "Cannot open the comb_file_config.txt.\n");
		exit( 0);
	}

	if( ( s_fp = fopen( COMBINE_FILE_STAT, "w")) == NULL)
	{
		fprintf( stderr, "Cannot open the comb_file_stat.txt.\n");
		exit( 0);
	}

	//Handle all the instances.
	ins_n = 1;
	file_cnt = 0;
	pos_cnt = 0;
	while( fscanf( c_fp, "%s%s%i", dir_path, taxi_id, &size_thr) != EOF)
	{
		printf( "Handling #instance %i ...\n", ins_n ++);

		//Open the directory.
		if( ( dir_p = opendir( dir_path)) == NULL)
		{
			fprintf( stderr, "Error occurs when opening %s.\n", dir_path);
			exit( 0);
		}

		//A temp file for storing the file names.
		if( ( o_fp = fopen( FILE_NAMES_TEMP, "w")) == NULL)
		{
			fprintf( stderr, "Cannot open the files_temp.txt\n");
			exit( 0);
		}
		
		//Read the directory.
		//Put all the file names in a temp file.
		/*t*/
		printf( "%s\n", dir_path);
		/*t*/
		cnt = 0;
		while( ( dir_r = readdir( dir_p)) != NULL)
		{
			if( dir_r->d_type == is_file)
			{
				//It corresponds to a file.
				//dir_r->d_name is the file name.
				fprintf( o_fp, "%s\n", dir_r->d_name);
				cnt ++;

				file_cnt ++;
			}
		}

		closedir( dir_p);		
		fclose( o_fp);
		
		//Re-read the file names from the temp files.
		if( ( i_fp = fopen( FILE_NAMES_TEMP, "r")) == NULL)
		{
			fprintf( stderr, "Cannot open the files_temp.txt\n");
			exit( 0);
		}
		
		file_names = ( char**)malloc( cnt * sizeof( char*));
		memset( file_names, 0, cnt * sizeof( char*));
		
		for( i=0; i<cnt; i++)
		{
			file_names[ i] = ( char*)malloc( MAX_FILENAME_LENG * sizeof( char));
			memset( file_names[ i], 0, MAX_FILENAME_LENG * sizeof( char));
			
			fscanf( i_fp, "%s", file_names[ i]);	
		}

		fclose( i_fp);

		//Sort the file names alphabetically.
		qsort( file_names, cnt, sizeof( char*), str_cmp);

		/*t/
		for( i=0; i<cnt; i++)
			printf( "%s\n", file_names[ i]);
		printf( "\n%i\n\n", cnt);
		/*t*/
		
		//Combine the files.
		//output_f = strcat( dir_path, "\/");
		//itoa( taxi_id, str_tmp, 10);
		//sprintf( str_tmp,  "%i", taxi_id);
		strcpy( output_f, file_loc);
		strcat( output_f, taxi_id);
		
		if( func_opt == 1)
			pos_n = combine_files_sub( dir_path, file_names, cnt, output_f, size_thr);
		else
			pos_n = collect_size( dir_path, file_names, cnt, output_f);

		pos_cnt += pos_n;

		/*t/
		printf( "Total number of positions: %i\n", pos_n);
		/*t*/
		
		//Print the statistics.
		fprintf( s_fp, "%s\t%i\n", dir_path, pos_n);

		//Release the resources.
		for( i=0; i<cnt; i++)
			free( file_names[ i]);
		free( file_names);
	}

	printf( "file_cnt: %i\n", file_cnt);
	printf( "pos_cnt: %i\n", pos_cnt);

	/*t*/
	printf( "Done!\n");
	/*t*/

	//Release the resources.
	fclose( c_fp);
	fclose( s_fp);
}


//Starting from 17 July, 2013.
/*
 *	The read_config function for the Min-Error problem.
 */
//emp_config_t* emp_read_config_v2( )



#endif
