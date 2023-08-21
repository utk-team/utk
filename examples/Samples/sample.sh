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
    
    $exec -n $N -m 64 -d 2 -s 20081976 -o $pts
    $spectra -i $pts --canceldc -o $spect

    python plot.py $pts $spect
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
        "BNOT"
    )

    for i in "${samplers[@]}"
    do
        echo $i
        plot_pointset $binpath/$i $n $outpath
    done
}

main $@