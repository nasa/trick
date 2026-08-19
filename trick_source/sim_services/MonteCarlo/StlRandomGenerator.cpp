
#include <stdexcept>
#include "trick/StlRandomGenerator.hh"

StlRandomGenerator::StlRandomGenerator(double          in_param_a,
                                       double          in_param_b,
                                       unsigned long   in_seed,
                                       StlDistribution in_dist_type,
                                       StlEngine       in_engine_type)
:   initialSeed(in_seed),
    engineEnum(in_engine_type),
    distEnum(in_dist_type)
{
}

StlRandomGenerator::~StlRandomGenerator()
{
}


///@details return a pointer-to-base-class for a new StlRandomGeneratorSub object
///         of the input-specified type
StlRandomGenerator*
StlRandomGeneratorFactory::newGenerator(
    double        in_param_a,
    double        in_param_b,
    unsigned long in_seed,
    StlRandomGenerator::StlDistribution in_dist_type,
    StlRandomGenerator::StlEngine       in_engine_type)
{

    switch (in_engine_type * 1000 + in_dist_type) {

    // ------------- TRICK_DEFAULT_ENGINE --------------------------------------

    case StlRandomGenerator::TRICK_DEFAULT_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< TrickDefaultEngine, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::TRICK_DEFAULT_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< TrickDefaultEngine, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::TRICK_DEFAULT_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< TrickDefaultEngine, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;


    // ------------- MINSTD_RAND_ENGINE ---------------------------------------------

    case StlRandomGenerator::MINSTD_RAND_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::minstd_rand, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::MINSTD_RAND_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::minstd_rand, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::MINSTD_RAND_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::minstd_rand, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;

    // ------------- MT19937_ENGINE ---------------------------------------------

    case StlRandomGenerator::MT19937_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::mt19937, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::MT19937_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::mt19937, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::MT19937_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::mt19937, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;

// these engines are all C++11 only
    // ------------- MT19937_64_ENGINE ---------------------------------------------

    case StlRandomGenerator::MT19937_64_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::mt19937_64, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::MT19937_64_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::mt19937_64, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::MT19937_64_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::mt19937_64, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;

    // ------------- RANLUX_24_BASE_ENGINE ---------------------------------------------

    case StlRandomGenerator::RANLUX_24_BASE_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux24_base, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::RANLUX_24_BASE_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux24_base, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::RANLUX_24_BASE_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux24_base, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;

    // ------------- RANLUX_44_BASE_ENGINE ---------------------------------------------

    case StlRandomGenerator::RANLUX_44_BASE_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux48_base, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::RANLUX_44_BASE_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux48_base, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::RANLUX_44_BASE_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux48_base, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;

    // ------------- RANLUX_24_ENGINE ---------------------------------------------

    case StlRandomGenerator::RANLUX_24_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux24, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::RANLUX_24_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux24, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::RANLUX_24_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux24, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;

    // ------------- RANLUX_44_ENGINE ---------------------------------------------

    case StlRandomGenerator::RANLUX_44_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux48, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::RANLUX_44_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux48, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::RANLUX_44_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::ranlux48, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;

    // ------------- KNUTH_B_ENGINE ---------------------------------------------

    case StlRandomGenerator::KNUTH_B_ENGINE*1000 + StlRandomGenerator::GAUSSIAN:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::knuth_b, std::normal_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::KNUTH_B_ENGINE*1000 + StlRandomGenerator::FLAT:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::knuth_b, std::uniform_real_distribution<double> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;
    case StlRandomGenerator::KNUTH_B_ENGINE*1000 + StlRandomGenerator::POISSON:
        return static_cast< StlRandomGenerator* > (
            new StlRandomGeneratorSub< std::knuth_b, std::poisson_distribution<int> >
                (in_param_a, in_param_b, in_seed, in_dist_type, in_engine_type)
        );
        break;

    default:
        throw std::invalid_argument(std::string("error: StlRandomGeneratorFactory::newGenerator called for unimplemented std::random  (engine,distribution) combination."));
    }
}
