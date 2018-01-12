################################################################
###########PARAMETERS###########################################
################################################################

SAMPLER=
INTEGRATION=
RANDOMIZE=
VALMIN=
VALMAX=
VALSTEP=
NAME=
LOG=true
VERBOSE=true
REWRITE=false
GNUPLOT=false
SORT=false
FORMAT=ebin

ID=$RANDOM

TEMP=`getopt -o s:d:i:r: -l sampler:,integrator:,randomize:,min:,max:,step:,sname:,silent,rewrite,gnuplot,log,sort -n 'generate_integration_graph_onfly' -- "$@"`
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

################################################################
###########FUNCTIONS############################################
################################################################

echo_parameters ()
{
	echo "[State]"
	echo "Sampler " ${SAMPLER}
	echo "Integrator " ${INTEGRATION}
	echo "Randomizer " ${RANDOMIZE}
	echo "From: 2^" ${VALMIN}
	echo "To: 2^" ${VALMAX}
	echo "Step: 2^" ${VALSTEP}
	echo "Rewriting: " ${REWRITE}
	echo "Gnuplotting: " ${GNUPLOT}
	echo "Sorting: " ${SORT}
	echo "Verbose: " ${VERBOSE}
	echo "Name: " ${SAMPLER_NAME}
}

check_parameters ()
{
	if [ -z "${SAMPLER}" ]
	then
		echo "No sampler specified"
		display_help
	fi

	if [ -z "${INTEGRATION}" ]
	then
		echo "No executable specified for integration computation"
		display_help
	fi

	if [ -z ${VALMIN} ]
	then
		echo "No min specified"
		display_help
	fi
	
	if [ -z ${VALMAX} ]
	then
		echo "No max specified"
		display_help
	fi
	
	if [ -z ${VALSTEP} ]
	then
		echo "No step specified"
		display_help
	fi
}


get_parameters ()
{
	eval set -- "$TEMP"

	while true; do
	case "$1" in

		-s | --sampler ) SAMPLER="$2"; shift 2 ;;
		-i | --integrator ) INTEGRATION="$2"; shift 2 ;;
		-r | --randomize ) RANDOMIZE="$2"; shift 2 ;;
		--min ) VALMIN="$2"; shift 2 ;;
		--max ) VALMAX="$2"; shift 2 ;;
		--step ) VALSTEP="$2"; shift 2 ;;
		--log ) LOG=true; shift ;;
		--silent ) VERBOSE=false; shift ;;
		--rewrite ) REWRITE=true; shift ;;
		--gnuplot ) GNUPLOT=true; shift ;;
		--sort ) SORT=true; shift ;;
		--sname ) NAME="$2"; shift 2;;
		-- ) shift; break ;;
		* ) break ;;
		
	esac
	done
	
	check_parameters
	
	if [ ! -z ${NAME} ]
	then
		SAMPLER_NAME=${NAME}
	else
		SAMPLER_NAME=`basename ${SAMPLER// /}`
	fi
	INTEGRATION_NAME=`basename ${INTEGRATION// /}`
	
	if [ ${VERBOSE} = true ]
	then
		echo_parameters
	fi
}

display_help ()
{
	echo_parameters
	
	echo "[Help]"
	echo "-s | --sampler <sampler_exe> \t the sampler executable"
	echo "WILL BE RUN AS ./sampler_exe -o [ouput file] -n [nb_points]"
	echo "-i | --integrator <integration_exe> \t the executable computing the integration"
	echo "WILL BE RUN AS ./integration_exe -i [input pointset] -o [ouput file]"
	echo "--min <int> \t\t\t the min value of the resulting graph"
	echo "--max <int>\t\t\t the max value of the resulting graph"
	echo "--step <float> \t\t\t the step between tho points of the graph (regular stepping)"
	echo "--log \t\t\t if set, the min value will be 2^min, the max 2^max and the step 2^step"
	echo "--silent \t\t set the script to silent mode" 
	echo "--rewrite \t\t if set, if a graph with the same name already exitsts, the results will be appened to it" 
	echo "--gnuplot \t\t if set, gnuplot will be called to generate a simple plot of the final graph"
	echo "--sort \t\t\t if set, the resulting graph will be sorted automatically"
	echo "--sname \t\t\t sets the sampler name. If not given, this name will be deduced from the sampler executable given"
	
	exit
}

generate_pointset()
{
	if [ ${VERBOSE} = true ]
	then
		date	
	fi
	if [ ! -e "pointsets_${SAMPLER_NAME}_n$1_${ID}.${FORMAT}" ]
	then
		if [ ! ${VERBOSE} = true ]
		then
			cmd="$SAMPLER -o pointsets_${SAMPLER_NAME}_n$1_${ID}.${FORMAT} -n $1 --silent"
		else
			cmd="$SAMPLER -o pointsets_${SAMPLER_NAME}_n$1_${ID}.${FORMAT} -n $1"
			echo $cmd 
		fi
		eval $cmd
	else
		echo "pointsets_${SAMPLER_NAME}_n$1_${ID}.${FORMAT} already exists"
	fi
}

randomize_pointset ()
{
	if [ ! -z ${RANDOMIZE} ]
	then
		date	
		cmd="$RANDOMIZE -i pointsets_${SAMPLER_NAME}_n${max_nbpts}_${ID}.${FORMAT} -o pointsets_${SAMPLER_NAME}_n${max_nbpts}_${ID}_tmp.${FORMAT}"
		echo $cmd 
		eval $cmd
		rm pointsets_${SAMPLER_NAME}_n${max_nbpts}_${ID}.${FORMAT}
		mv pointsets_${SAMPLER_NAME}_n${max_nbpts}_${ID}_tmp.${FORMAT} pointsets_${SAMPLER_NAME}_n${max_nbpts}_${ID}.${FORMAT}
	fi
}

integrate_pointset ()
{
	if [ ${VERBOSE} = true ]
	then
		date	
	fi
	if [ ! ${VERBOSE} = true ]
	then
		cmd="$INTEGRATION -s $1 -i pointsets_${SAMPLER_NAME}_n${max_nbpts}_${ID}.${FORMAT} -o ${TMPFILE} --silent"
	else
		cmd="$INTEGRATION -s $1 -i pointsets_${SAMPLER_NAME}_n${max_nbpts}_${ID}.${FORMAT} -o ${TMPFILE}"
		echo $cmd 
	fi
	eval $cmd
}

generate_plot ()
{
	echo "set logscale xy" >> gnuplot_${ID}.txt
	echo "set terminal pngcairo fontscale 1.0 dashed size 1000, 700" >> gnuplot_${ID}.txt
	echo "set xlabel \"number of points\"" >> gnuplot_${ID}.txt
	echo "set output \"${INTEGRATION_PNG}\"" >> gnuplot_${ID}.txt
	echo "plot \"${INTEGRATION_GRAPH}\" u 1:3 w lp title \"$SAMPLER_NAME var integration\" lw 2" >> gnuplot_${ID}.txt
	echo "set output \"${INTEGRATION_PNG}\"" >> gnuplot_${ID}.txt
	echo "replot 1/(x*sqrt(x)) lt 3 lc rgb \"green\", 1/x lt 3 lc rgb \"blue\"" >> gnuplot_${ID}.txt

	gnuplot gnuplot_${ID}.txt
	rm gnuplot_${ID}.txt
}

################################################################
###########SCRIPT CODE##########################################
################################################################

get_parameters

INTEGRATION_GRAPH=pointsets_${SAMPLER_NAME}_integration_graph_${INTEGRATION_NAME}_${ID}.txt
INTEGRATION_PNG=pointsets_${SAMPLER_NAME}_integration_graph_${INTEGRATION_NAME}_${ID}.png
TMPFILE=`date "+%d-%m-%y-"`"-generate_integration_fromsequence-"${SAMPLER_NAME}"-"${INTEGRATION_NAME}"-"${VALMIN}"-"${VALMAX}"-"${VALSTEP}"_${ID}.txt"

start_time=$(date +'%s')

tmp=`echo "e( ${VALMAX} * l(2) )" | bc -l`
max_nbpts=`echo "2*$tmp/2 + 1" | bc`
generate_pointset ${max_nbpts}

if [ ${REWRITE} = true ] || [ ! -e ${INTEGRATION_GRAPH} ]
then
	current=${VALMIN}
	goon=`echo "${current}<=${VALMAX}" | bc`
	while [ $goon -eq 1 ]
	do
		if [ $LOG ]
		then
			tmp=`echo "e( ${current} * l(2) )" | bc -l`
			real_current=`echo "2*$tmp/2 + 1" | bc`
		else
			real_current=${current}
		fi
		
		randomize_pointset ${real_current}
		if [ ! $? = "0" ]
		then
			echo "Error while randomizing pointset ${real_current}"
			exit
		fi
		
		integrate_pointset ${real_current}
		if [ ! $? = "0" ]
		then
			echo "Error while computing integration on pointset ${real_current}"
			exit
		fi
		
		echo "Copy to " ${INTEGRATION_GRAPH}
		cat ${TMPFILE} >> ${INTEGRATION_GRAPH}
		rm ${TMPFILE}
		#rm "pointsets_${SAMPLER_NAME}_n${real_current}_${ID}.${FORMAT}"

		current=`echo "${current}+${VALSTEP}" | bc -l`
		goon=`echo "${current}<=${VALMAX}" | bc`
		
	done
else
	echo "Graph " ${INTEGRATION_GRAPH} " already exists, aborting"
fi

if [ ${SORT} = true ]
then
	if [ ${VERBOSE} = true ]
	then
		echo "Sorting ..."
	fi
	sort -n ${INTEGRATION_GRAPH} >> TMP_${INTEGRATION_GRAPH}
	mv TMP_${INTEGRATION_GRAPH} ${INTEGRATION_GRAPH}
fi

if [ ${GNUPLOT} = true ]
then
	if [ ${VERBOSE} = true ]
	then
		echo "Plotting ..."
	fi
	generate_plot
fi

endtime=$(date +'%s')
time_elapsed=$(($endtime-$start_time))
minutes=$(($time_elapsed/60))
seconds=$(($time_elapsed-$minutes*60))
echo "Computed in $minutes min $seconds sc"
