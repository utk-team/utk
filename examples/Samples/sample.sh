extract_data_until_hash() {
    input_file="$1"
    output_file="$2"
    extract_data=false

    while IFS= read -r line; do
        if [[ "$line" == "#"* ]]; then
            break
        fi

        if [ "$extract_data" = true ]; then
            echo "$line" >> "$output_file"
        fi

        if [ "$extract_data" = false ]; then
            extract_data=true
        fi
    done < "$input_file"
}

plot_pointset()
{
    # Parameters :
    # - $1 sampler file
    # - $2 Npts
    # - $3 output

    local exec=$1
    
    local N=$2
    local out=$3
    local name="$(basename $exec)"
    
    local direxec="$(dirname $exec)"
    local spectra="$(realpath $direxec/../metrics/Spectrum)"

    local pts="$out/$name.dat"
    local spect="$out/${name}_spectrum.dat"
    
    $exec -n $N -m 256 -d 2 -s 20081976 -o $pts
    $spectra -i $pts --canceldc -o $spect

    extract_data_until_hash $pts tmp.dat

    python plot.py tmp.dat $spect
    filename=$(basename -- "$fullfile")
    extension="${filename##*.}"
    filename="${filename%.*}"
    mv tmp.png ${pts%.*}.png
    rm tmp.dat    
}

main()
{
    binpath="../../build/src/samplers/"
    outpath="../../doc-sources/figs/samplers"
    n=1024

    if [ $# -ge 1 ]; then
        binpath=$1
    fi

    if [ $# -ge 2 ]; then
        outpath=$2
    fi

    if [ $# -ge 3 ]; then
        n=$3
    fi

    mkdir -p $outpath

    samplers=(
        "Whitenoise"
        "Stratified"
        "RegularGrid"
        "HexagonalGrid"
        "DartThrowing"
        "FastPoisson"
        "ProjectiveBlueNoise"
        "Step"
        "SinglePeak"
        "Fattal"
        "Hammersley"
        "Halton"
        "LutLDBN"
        "Niederreiter"
        "Faure"
        "ForcedRandom"
        "PMJ"
        "Sobol"
        "Rank1"
        "Korobov"
        "Kronecker"
        "ART"
        "Penrose"
        "AAPatterns"
        "CMJ"
        "NRooks"
        "PeriodicBNOT"
        "BoundedBNOT"
    )

    for i in "${samplers[@]}"
    do
        echo $i
        plot_pointset $binpath/$i $n $outpath
    done
}

main $@