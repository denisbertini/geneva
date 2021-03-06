/********************************************************************************
 *
 * This file is part of the Geneva library collection. The following license
 * applies to this file:
 *
 * ------------------------------------------------------------------------------
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ------------------------------------------------------------------------------
 *
 * Note that other files in the Geneva library collection may use a different
 * license. Please see the licensing information in each file.
 *
 ********************************************************************************
 *
 * Geneva was started by Dr. Rüdiger Berlich and was later maintained together
 * with Dr. Ariel Garcia under the auspices of Gemfony scientific. For further
 * information on Gemfony scientific, see http://www.gemfomy.eu .
 *
 * The majority of files in Geneva was released under the Apache license v2.0
 * in February 2020.
 *
 * See the NOTICE file in the top-level directory of the Geneva library
 * collection for a list of contributors and copyright information.
 *
 ********************************************************************************/

#pragma once

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard header files go here
#include <random>

// Boost header files go here
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/lexical_cast.hpp>

// Geneva header files go here
#include "common/GExceptions.hpp"
#include "hap/GRandomBase.hpp"
#include "hap/GRandomT.hpp"
#include "geneva/GMutableParameterI.hpp"
#include "geneva/GObject.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The purpose of this class is to provide a common base for all parameter classes, so
 * that a GParameterSet can be built from different parameter types. The class also
 * defines the interface that needs to be implemented by parameter classes.
 */
class GParameterBase
    : public GObject
    , public GMutableParameterI
{
    ///////////////////////////////////////////////////////////////////////
    friend class boost::serialization::access;

    template<typename Archive>
    void serialize(Archive &ar, const unsigned int) {
        using boost::serialization::make_nvp;

        ar
        & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GObject)
        & BOOST_SERIALIZATION_NVP(m_adaptionsActive)
        & BOOST_SERIALIZATION_NVP(m_randomInitializationBlocked)
        & BOOST_SERIALIZATION_NVP(m_parameterName);
    }
    ///////////////////////////////////////////////////////////////////////
public:

    /*********************************************************************/
    // Defaulted constructors, destructor and assignment operators

    G_API_GENEVA GParameterBase() = default;
    G_API_GENEVA GParameterBase(GParameterBase const &) = default;
    G_API_GENEVA GParameterBase(GParameterBase &&) = default;

    G_API_GENEVA ~GParameterBase() override = default;

    G_API_GENEVA GParameterBase& operator=(GParameterBase const&) = default;
    G_API_GENEVA GParameterBase& operator=(GParameterBase &&) = default;

    /*********************************************************************/

    /** @brief The adaption interface */
    G_API_GENEVA std::size_t adapt(Gem::Hap::GRandomBase &) override;

    /** @brief Update adaptors depending on the number of iterations without improvement */
    G_API_GENEVA bool updateAdaptorsOnStall(std::size_t);

    /** @brief Retrieves information from an adaptor on a given property */
    G_API_GENEVA void queryAdaptor(
        const std::string &adaptorName
        , const std::string &property
        , std::vector<boost::any> &data
    ) const;

    /** @brief Switches on adaptions for this object */
    G_API_GENEVA bool setAdaptionsActive();
    /** @brief Disables adaptions for this object */
    G_API_GENEVA bool setAdaptionsInactive();
    /** @brief Determines whether adaptions are performed for this object */
    G_API_GENEVA bool adaptionsActive() const;
    /** @brief Determines whether adaptions are inactive for this object */
    G_API_GENEVA bool adaptionsInactive() const;

    /** @brief Triggers random initialization of the parameter(-collection) */
    virtual G_API_GENEVA bool randomInit(
        const activityMode &
        , Gem::Hap::GRandomBase &
    );

    /** @brief Allows to identify whether we are dealing with a collection or an individual parameter */
    G_API_GENEVA bool isIndividualParameter() const;
    /** @brief Allows to identify whether we are dealing with a collection or an individual parameter */
    G_API_GENEVA bool isParameterCollection() const;

    /** @brief Allows to assign a name to this parameter */
    G_API_GENEVA void setParameterName(const std::string &);
    /** @brief Allows to retrieve the name of this parameter */
    G_API_GENEVA std::string getParameterName() const;

    /** @brief Checks whether this object matches a given activity mode */
    G_API_GENEVA bool amMatch(const activityMode &) const;
    /** @brief Returns true on the case of an activity mode mismatch */
    G_API_GENEVA bool amMismatch(const activityMode &) const;

    /** @brief Checks whether this object matches a given activity mode and is modifiable */
    G_API_GENEVA bool modifiableAmMatchOrHandover(const activityMode &) const;

    /***************************************************************************/
    /**
     * Allows to count parameters of a specific type. This function is a trap, needed to
     * catch attempts to use this function with unsupported types. Use the supplied
     * specializations instead.
     *
     * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
     * @return The number of parameters of a given Type
     */
    template<typename par_type>
    std::size_t countParameters(
        activityMode am
    ) const {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::countParameters()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );

        // Make the compiler happy
        return (std::size_t) 0;
    }

    /***************************************************************************/
    /**
     * Allows to add all boundaries if parameters of a specific type to the vectors. This
     * function is a trap, needed to catch streamlining attempts with unsupported types.
     * Use the supplied specializations instead.
     *
     * @oaram lBndVec The vector with lower boundaries of parameters
     * @oaram uBndVec The vector with upper boundaries of parameters
     */
    template<typename par_type>
    void boundaries(
        std::vector<par_type> &lBndVec
        , std::vector<par_type> &uBndVec
        , activityMode am
    ) const {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::boundaries(std::vector<>&)" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Allows to add all parameters of a specific type to the vector. This function is a
     * trap, needed to catch streamlining attempts with unsupported types. Use the supplied
     * specializations instead.
     *
     * @oaram parVec The vector to which the items should be added
     */
    template<typename par_type>
    void streamline(
        std::vector<par_type> &parVec
        , activityMode am
    ) const {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::streamline(std::vector<par_type>&)" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Allows to add all parameters of a specific type to the map. This function is a
     * trap, needed to catch streamlining attempts with unsupported types. Use the supplied
     * specializations instead.
     *
     * @oaram parVec The vector to which the items should be added
     */
    template<typename par_type>
    void streamline(
        std::map<std::string, std::vector<par_type>> &parVec
        , activityMode am
    ) const {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::streamline(std::map<std::string, std::vec<par_type>>)" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Allows to assign the parameters inside of a vector the corresponding parameter objects.
     * This function is a trap, needed to catch attempts to use this function with unsupported
     * types. Use the supplied specializations instead.
     *
     * @param parVec The vector with the parameters to be assigned to the object
     * @param pos The position from which parameters will be taken (will be updated by the call)
     */
    template<typename par_type>
    void assignValueVector(
        const std::vector<par_type> &parVec
        , std::size_t &pos
        , activityMode am
    ) {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::assignValueVector()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Assigns values from a std::map<std::string, std::vector<par_type>> to the parameter
     *
     * @param parMao The map with the parameters to be assigned to the object
     */
    template<typename par_type>
    void assignValueVectors(
        const std::map<std::string, std::vector<par_type>> &parMap
        , activityMode am
    ) {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::assignValueVectors()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Multiplication with a random value in a given range
     */
    template<typename par_type>
    void multiplyByRandom(
        const par_type &min
        , const par_type &max
        , activityMode am
        , Gem::Hap::GRandomBase &
    ) {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::multiplyByRandom()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Multiplication with a random value in the range [0, 1[
     */
    template<typename par_type>
    void multiplyByRandom(
        activityMode am
        , Gem::Hap::GRandomBase &
    ) {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::multiplyByRandom()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Multiplication with a constant value
     */
    template<typename par_type>
    void multiplyBy(
        par_type val
        , activityMode am
    ) {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::multiplyBy()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Initializes all parameters of a given type with a constant value
     */
    template<typename par_type>
    void fixedValueInit(
        par_type val
        , activityMode am
    ) {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::fixedValueInit()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Adds the parameters of another GParameterSet object to this one
     */
    template<typename par_type>
    void add(
        const std::shared_ptr<GParameterBase> &p
        , activityMode am
    ) {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::add()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/
    /**
     * Subtracts the parameters of another GParameterSet object from this one
     */
    template<typename par_type>
    void subtract(
        const std::shared_ptr<GParameterBase> &p
        , activityMode am
    ) {
        throw gemfony_exception(
            g_error_streamer(
                DO_LOG
                , time_and_place
            )
                << "In GParameterBase::subtract()" << std::endl
                << "Function called for unsupported type!" << std::endl
        );
    }

    /***************************************************************************/

    /** @brief Specifies that no random initialization should occur anymore */
    G_API_GENEVA void blockRandomInitialization();
    /** @brief Makes random initialization possible */
    G_API_GENEVA void allowRandomInitialization();
    /** @brief Checks whether initialization has been blocked */
    G_API_GENEVA bool randomInitializationBlocked() const;

    /** @brief Convenience function so we do not need to always cast derived classes */
    virtual G_API_GENEVA bool hasAdaptor() const BASE;

    /** @brief Converts the local data to a boost::property_tree node */
    virtual G_API_GENEVA void toPropertyTree(pt::ptree &, const std::string &) const BASE = 0;

    /** @brief Lets the audience know whether this is a leaf or a branch object */
    virtual G_API_GENEVA bool isLeaf() const BASE;

    /***************************************************************************/
    /**
     * This function converts a GParameterBase std::shared_ptr to the target type.  Note that this
     * template will only be accessible to the compiler if GParameterBase is a base type of load_type.
     *
     * @param load_ptr A std::shared_ptr<load_type> to the item to be converted
     * @param dummy A dummy argument needed for std::enable_if and type_traits magic
     * @return A std::shared_ptr holding the converted object
     */
    template<typename load_type>
    std::shared_ptr<load_type> parameterbase_cast(
        std::shared_ptr<GParameterBase> load_ptr
        , typename std::enable_if<std::is_base_of<Gem::Geneva::GParameterBase, load_type>::value>::type *dummy = nullptr
    ) const {
#ifdef DEBUG
        std::shared_ptr<load_type> p = std::dynamic_pointer_cast<load_type>(load_ptr);
        if (p) { return p; }
        else {
            throw gemfony_exception(
                g_error_streamer(
                    DO_LOG
                    , time_and_place
                )
                    << "In std::shared_ptr<load_type> GParameterBase::parameterbase_cast<load_type>() :" << std::endl
                    << "Invalid conversion with load_type = " << typeid(load_type).name() << std::endl
            );
        }
#else
        return std::static_pointer_cast<load_type>(load_ptr);
#endif
    }


protected:
    /***************************************************************************/
    /** @brief Count the number of float parameters */
    virtual G_API_GENEVA std::size_t countFloatParameters(const activityMode &am) const BASE;

    /** @brief Count the number of double parameters */
    virtual G_API_GENEVA std::size_t countDoubleParameters(const activityMode &am) const BASE;

    /** @brief Count the number of std::int32_t parameters */
    virtual G_API_GENEVA std::size_t countInt32Parameters(const activityMode &am) const BASE;

    /** @brief Count the number of bool parameters */
    virtual G_API_GENEVA std::size_t countBoolParameters(const activityMode &am) const BASE;

    /** @brief Attach boundaries of type float to the vectors */
    virtual G_API_GENEVA void floatBoundaries(
        std::vector<float> &, std::vector<float> &, const activityMode &
    ) const BASE;

    /** @brief Attach boundaries of type double to the vectors */
    virtual G_API_GENEVA void doubleBoundaries(
        std::vector<double> &, std::vector<double> &, const activityMode &
    ) const BASE;

    /** @brief Attach boundaries of type std::int32_t to the vectors */
    virtual G_API_GENEVA void int32Boundaries(
        std::vector<std::int32_t> &, std::vector<std::int32_t> &, const activityMode &
    ) const BASE;

    /** @brief Attach boundaries of type bool to the vectors */
    virtual G_API_GENEVA void booleanBoundaries(
        std::vector<bool> &, std::vector<bool> &, const activityMode &
    ) const BASE;

    /** @brief Attach parameters of type float to the vector */
    virtual G_API_GENEVA void floatStreamline(std::vector<float> &, const activityMode &) const BASE;

    /** @brief Attach parameters of type double to the vector */
    virtual G_API_GENEVA void doubleStreamline(std::vector<double> &, const activityMode &) const BASE;

    /** @brief Attach parameters of type std::int32_t to the vector */
    virtual G_API_GENEVA void int32Streamline(std::vector<std::int32_t> &, const activityMode &) const BASE;

    /** @brief Attach parameters of type bool to the vector */
    virtual G_API_GENEVA void booleanStreamline(std::vector<bool> &, const activityMode &) const BASE;

    /** @brief Attach parameters of type float to the map */
    virtual G_API_GENEVA void floatStreamline(
        std::map<std::string, std::vector<float>> &, const activityMode &
    ) const BASE;

    /** @brief Attach parameters of type double to the map */
    virtual G_API_GENEVA void doubleStreamline(
        std::map<std::string, std::vector<double>> &, const activityMode &
    ) const BASE;

    /** @brief Attach parameters of type std::int32_t to the map */
    virtual G_API_GENEVA void int32Streamline(
        std::map<std::string, std::vector<std::int32_t>> &, const activityMode &
    ) const BASE;

    /** @brief Attach parameters of type bool to the map */
    virtual G_API_GENEVA void booleanStreamline(
        std::map<std::string, std::vector<bool>> &, const activityMode &
    ) const BASE;

    /** @brief Assigns part of a value vector to the parameter */
    virtual G_API_GENEVA void assignFloatValueVector(
        const std::vector<float> &, std::size_t &, const activityMode &
    ) BASE;

    /** @brief Assigns part of a value vector to the parameter */
    virtual G_API_GENEVA void assignDoubleValueVector(
        const std::vector<double> &, std::size_t &, const activityMode &
    ) BASE;

    /** @brief Assigns part of a value vector to the parameter */
    virtual G_API_GENEVA void assignInt32ValueVector(
        const std::vector<std::int32_t> &, std::size_t &, const activityMode &
    ) BASE;

    /** @brief Assigns part of a value vector to the parameter */
    virtual G_API_GENEVA void assignBooleanValueVector(
        const std::vector<bool> &, std::size_t &, const activityMode &
    ) BASE;

    /** @brief Assigns part of a value vector to the parameter */
    virtual G_API_GENEVA void assignFloatValueVectors(
        const std::map<std::string, std::vector<float>> &, const activityMode &
    ) BASE;

    /** @brief Assigns part of a value vector to the parameter */
    virtual G_API_GENEVA void assignDoubleValueVectors(
        const std::map<std::string, std::vector<double>> &, const activityMode &
    ) BASE;

    /** @brief Assigns part of a value vector to the parameter */
    virtual G_API_GENEVA void assignInt32ValueVectors(
        const std::map<std::string, std::vector<std::int32_t>> &, const activityMode &
    ) BASE;

    /** @brief Assigns part of a value vector to the parameter */
    virtual G_API_GENEVA void assignBooleanValueVectors(
        const std::map<std::string, std::vector<bool>> &, const activityMode &
    ) BASE;

    /** @brief Multiplication with a random value in a given range */
    virtual G_API_GENEVA void floatMultiplyByRandom(
        const float &min, const float &max, const activityMode &am, Gem::Hap::GRandomBase &
    ) BASE;

    /** @brief Multiplication with a random value in a given range */
    virtual G_API_GENEVA void doubleMultiplyByRandom(
        const double &min, const double &max, const activityMode &am, Gem::Hap::GRandomBase &
    ) BASE;

    /** @brief Multiplication with a random value in a given range */
    virtual G_API_GENEVA void int32MultiplyByRandom(
        const std::int32_t &min, const std::int32_t &max, const activityMode &am, Gem::Hap::GRandomBase &
    ) BASE;

    /** @brief Multiplication with a random value in a given range */
    virtual G_API_GENEVA void booleanMultiplyByRandom(
        const bool &min, const bool &max, const activityMode &am, Gem::Hap::GRandomBase &
    ) BASE;

    /** @brief Multiplication with a random value in the range [0,1[ */
    virtual G_API_GENEVA void floatMultiplyByRandom(const activityMode &am, Gem::Hap::GRandomBase &) BASE;

    /** @brief Multiplication with a random value in the range [0,1[ */
    virtual G_API_GENEVA void doubleMultiplyByRandom(const activityMode &am, Gem::Hap::GRandomBase &) BASE;

    /** @brief Multiplication with a random value in the range [0,1[ */
    virtual G_API_GENEVA void int32MultiplyByRandom(const activityMode &am, Gem::Hap::GRandomBase &) BASE;

    /** @brief Multiplication with a random value in the range [0,1[ */
    virtual G_API_GENEVA void booleanMultiplyByRandom(const activityMode &am, Gem::Hap::GRandomBase &) BASE;

    /** @brief Multiplication with a constant value */
    virtual G_API_GENEVA void floatMultiplyBy(const float &value, const activityMode &am) BASE;

    /** @brief Multiplication with a constant value */
    virtual G_API_GENEVA void doubleMultiplyBy(const double &value, const activityMode &am) BASE;

    /** @brief Multiplication with a constant value */
    virtual G_API_GENEVA void int32MultiplyBy(const std::int32_t &value, const activityMode &am) BASE;

    /** @brief Multiplication with a const value */
    virtual G_API_GENEVA void booleanMultiplyBy(const bool &value, const activityMode &am) BASE;

    /** @brief Initialization with a constant value */
    virtual G_API_GENEVA void floatFixedValueInit(const float &value, const activityMode &am) BASE;

    /** @brief Initialization with a constant value */
    virtual G_API_GENEVA void doubleFixedValueInit(const double &value, const activityMode &am) BASE;

    /** @brief Initialization with a constant value */
    virtual G_API_GENEVA void int32FixedValueInit(const std::int32_t &value, const activityMode &am) BASE;

    /** @brief Initialization with a const value */
    virtual G_API_GENEVA void booleanFixedValueInit(const bool &value, const activityMode &am) BASE;

    /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
    virtual G_API_GENEVA void floatAdd(std::shared_ptr<GParameterBase>, const activityMode &am) BASE;

    /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
    virtual G_API_GENEVA void doubleAdd(std::shared_ptr<GParameterBase>, const activityMode &am) BASE;

    /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
    virtual G_API_GENEVA void int32Add(std::shared_ptr<GParameterBase>, const activityMode &am) BASE;

    /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
    virtual G_API_GENEVA void booleanAdd(std::shared_ptr<GParameterBase>, const activityMode &am) BASE;

    /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
    virtual G_API_GENEVA void floatSubtract(std::shared_ptr<GParameterBase>, const activityMode &am) BASE;

    /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
    virtual G_API_GENEVA void doubleSubtract(std::shared_ptr<GParameterBase>, const activityMode &am) BASE;

    /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
    virtual G_API_GENEVA void int32Subtract(std::shared_ptr<GParameterBase>, const activityMode &am) BASE;

    /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
    virtual G_API_GENEVA void booleanSubtract(std::shared_ptr<GParameterBase>, const activityMode &am) BASE;

    /***************************************************************************/
    /** @brief Loads the data of another GObject */
    G_API_GENEVA void load_(const GObject *) override;

    /** @brief Allow access to this classes compare_ function */
    friend void Gem::Common::compare_base_t<GParameterBase>(
        GParameterBase const &
        , GParameterBase const &
        , Gem::Common::GToken &
    );

    /** @brief Searches for compliance with expectations with respect to another object of the same type */
    G_API_GENEVA void compare_(
        const GObject & // the other object
        , const Gem::Common::expectation & // the expectation for this object, e.g. equality
        , const double & // the limit for allowed deviations of floating point types
    ) const override;

    /** @brief Triggers random initialization of the parameter(-collection) */
    virtual G_API_GENEVA bool randomInit_(
        const activityMode &
        , Gem::Hap::GRandomBase &
    ) BASE = 0;

    /** @brief Applies modifications to this object. This is needed for testing purposes */
    G_API_GENEVA bool modify_GUnitTests_() override;
    /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
    G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests_() override;
    /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
    G_API_GENEVA void specificTestsFailuresExpected_GUnitTests_() override;

private:
    /***************************************************************************/
    /** @brief Emits a name for this class / object */
    G_API_GENEVA std::string name_() const override;
    /** @brief Creates a deep clone of this object */
    G_API_GENEVA GObject *clone_() const override = 0;

    /** @brief The actual adaption logic */
    virtual G_API_GENEVA std::size_t adapt_(Gem::Hap::GRandomBase &) BASE = 0;
    /** @brief Triggers updates when the optimization process has stalled */
    virtual G_API_GENEVA bool updateAdaptorsOnStall_(std::size_t) BASE = 0;

    /** @brief Retrieves information from an adaptor on a given property */
    virtual G_API_GENEVA void queryAdaptor_(
        const std::string &adaptorName
        , const std::string &property
        , std::vector<boost::any> &data
    ) const BASE = 0;

    /** @brief Allows to identify whether we are dealing with a collection or an individual parameter */
    virtual G_API_GENEVA bool isIndividualParameter_() const BASE;

    /***************************************************************************/
    bool m_adaptionsActive = true; ///< Specifies whether adaptions of this object should be carried out
    bool m_randomInitializationBlocked = false; ///< Specifies that this object should not be initialized again
    std::string m_parameterName = Gem::Common::to_string(boost::uuids::random_generator()()); ///< A name assigned to this parameter object
};

/******************************************************************************/
/**
 * Specializations of some template functions
 */

/******************************************************************************/
/**
 * Allows to add all parameters of type float to the vector.
 *
 * @oaram parVec The vector to which the items should be added
 */
template<>
inline void GParameterBase::streamline<float>(
    std::vector<float> &parVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatStreamline(
            parVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to add all parameters of type double to the vector.
 *
 * @oaram parVec The vector to which the items should be added
 */
template<>
inline void GParameterBase::streamline<double>(
    std::vector<double> &parVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleStreamline(
            parVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to add all parameters of type std::int32_t to the vector.
 *
 * @oaram parVec The vector to which the items should be added
 */
template<>
inline void GParameterBase::streamline<std::int32_t>(
    std::vector<std::int32_t> &parVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32Streamline(
            parVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to add all parameters of type bool to the vector.
 *
 * @oaram parVec The vector to which the items should be added
 */
template<>
inline void GParameterBase::streamline<bool>(
    std::vector<bool> &parVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->booleanStreamline(
            parVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to add all parameters of type float to the map.
 *
 * @oaram parVec The map to which the items should be added
 */
template<>
inline void GParameterBase::streamline<float>(
    std::map<std::string, std::vector<float>> &parVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatStreamline(
            parVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to add all parameters of type double to the map.
 *
 * @oaram parVec The vector to which the items should be added
 */
template<>
inline void GParameterBase::streamline<double>(
    std::map<std::string, std::vector<double>> &parVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleStreamline(
            parVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to add all parameters of type std::int32_t to the map.
 *
 * @oaram parVec The vector to which the items should be added
 */
template<>
inline void GParameterBase::streamline<std::int32_t>(
    std::map<std::string, std::vector<std::int32_t>> &parVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32Streamline(
            parVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to add all parameters of type bool to the map.
 *
 * @oaram parVec The vector to which the items should be added
 */
template<>
inline void GParameterBase::streamline<bool>(
    std::map<std::string, std::vector<bool>> &parVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->booleanStreamline(
            parVec
            , am
        );
    }
}


/******************************************************************************/
/**
 * Allows to retrieve the values of lower and upper boundaries of type float
 *
 * @param lBndVec A vector of lower double parameter boundaries
 * @param uBndVec A vector of upper double parameter boundaries
 */
template<>
inline void GParameterBase::boundaries<float>(
    std::vector<float> &lBndVec
    , std::vector<float> &uBndVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatBoundaries(
            lBndVec
            , uBndVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to retrieve the values of lower and upper boundaries of type double
 *
 * @param lBndVec A vector of lower double parameter boundaries
 * @param uBndVec A vector of upper double parameter boundaries
 */
template<>
inline void GParameterBase::boundaries<double>(
    std::vector<double> &lBndVec
    , std::vector<double> &uBndVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleBoundaries(
            lBndVec
            , uBndVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to retrieve the values of lower and upper boundaries of type std::int32_t
 *
 * @param lBndVec A vector of lower std::int32_t parameter boundaries
 * @param uBndVec A vector of upper std::int32_t parameter boundaries
 */
template<>
inline void GParameterBase::boundaries<std::int32_t>(
    std::vector<std::int32_t> &lBndVec
    , std::vector<std::int32_t> &uBndVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32Boundaries(
            lBndVec
            , uBndVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to retrieve the values of lower and upper boundaries of type bool
 *
 * @param lBndVec A vector of lower bool parameter boundaries
 * @param uBndVec A vector of upper bool parameter boundaries
 */
template<>
inline void GParameterBase::boundaries<bool>(
    std::vector<bool> &lBndVec
    , std::vector<bool> &uBndVec
    , activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->booleanBoundaries(
            lBndVec
            , uBndVec
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to count parameters of type float.
 *
 * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
 * @return The number of parameters of type float
 */
template<>
inline std::size_t GParameterBase::countParameters<float>(
    activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        return this->countFloatParameters(am);
    } else {
        return 0;
    }
}

/******************************************************************************/
/**
 * Allows to count parameters of type double.
 *
 * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
 * @return The number of parameters of type double
 */
template<>
inline std::size_t GParameterBase::countParameters<double>(
    activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        return this->countDoubleParameters(am);
    } else {
        return 0;
    }
}

/******************************************************************************/
/**
 * Allows to count parameters of type std::int32_t.
 *
 * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
 * @return The number of parameters of type std::int32_t
 */
template<>
inline std::size_t GParameterBase::countParameters<std::int32_t>(
    activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        return this->countInt32Parameters(am);
    } else {
        return 0;
    }
}

/******************************************************************************/
/**
 * Allows to count parameters of type bool.
 *
 * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
 * @return The number of parameters of type bool
 */
template<>
inline std::size_t GParameterBase::countParameters<bool>(
    activityMode am
) const {
    if (this->modifiableAmMatchOrHandover(am)) {
        return this->countBoolParameters(am);
    } else {
        return 0;
    }
}

/******************************************************************************/
/**
 * Allows to assign the parameters inside of a vector the corresponding parameter objects.
 *
 * @param parVec The vector with the parameters to be assigned to the object
 * @param pos The position from which parameters will be taken (will be updated by the call)
 */
template<>
inline void GParameterBase::assignValueVector<float>(
    const std::vector<float> &parVec
    , std::size_t &pos
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->assignFloatValueVector(
            parVec
            , pos
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to assign the parameters inside of a vector the corresponding parameter objects.
 *
 * @param parVec The vector with the parameters to be assigned to the object
 * @param pos The position from which parameters will be taken (will be updated by the call)
 */
template<>
inline void GParameterBase::assignValueVector<double>(
    const std::vector<double> &parVec
    , std::size_t &pos
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->assignDoubleValueVector(
            parVec
            , pos
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to assign the parameters inside of a vector the corresponding parameter objects.
 *
 * @param parVec The vector with the parameters to be assigned to the object
 * @param pos The position from which parameters will be taken (will be updated by the call)
 */
template<>
inline void GParameterBase::assignValueVector<std::int32_t>(
    const std::vector<std::int32_t> &parVec
    , std::size_t &pos
    , activityMode am
) {
    this->assignInt32ValueVector(
        parVec
        , pos
        , am
    );
}

/******************************************************************************/
/**
 * Allows to assign the parameters inside of a vector the corresponding parameter objects.
 *
 * @param parVec The vector with the parameters to be assigned to the object
 * @param pos The position from which parameters will be taken (will be updated by the call)
 */
template<>
inline void GParameterBase::assignValueVector<bool>(
    const std::vector<bool> &parVec
    , std::size_t &pos
    , activityMode am
) {
    this->assignBooleanValueVector(
        parVec
        , pos
        , am
    );
}

/******************************************************************************/
/**
 * Allows to assign the parameters inside of a map to the corresponding parameter objects.
 *
 * @param parMap The vector with the parameters to be assigned to the object
 */
template<>
inline void GParameterBase::assignValueVectors<float>(
    const std::map<std::string, std::vector<float>> &parMap
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->assignFloatValueVectors(
            parMap
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to assign the parameters inside of a map to the corresponding parameter objects.
 *
 * @param parMap The vector with the parameters to be assigned to the object
 */
template<>
inline void GParameterBase::assignValueVectors<double>(
    const std::map<std::string, std::vector<double>> &parMap
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->assignDoubleValueVectors(
            parMap
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to assign the parameters inside of a map to the corresponding parameter objects.
 *
 * @param parMap The vector with the parameters to be assigned to the object
 */
template<>
inline void GParameterBase::assignValueVectors<std::int32_t>(
    const std::map<std::string, std::vector<std::int32_t>> &parMap
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->assignInt32ValueVectors(
            parMap
            , am
        );
    }
}

/******************************************************************************/
/**
 * Allows to assign the parameters inside of a map to the corresponding parameter objects.
 *
 * @param parMap The vector with the parameters to be assigned to the object
 */
template<>
inline void GParameterBase::assignValueVectors<bool>(
    const std::map<std::string, std::vector<bool>> &parMap
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->assignBooleanValueVectors(
            parMap
            , am
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a random value in a given range
 */
template<>
inline void GParameterBase::multiplyByRandom<float>(
    const float &min
    , const float &max
    , activityMode am
    , Gem::Hap::GRandomBase &gr
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatMultiplyByRandom(
            min
            , max
            , am
            , gr
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a random value in a given range
 */
template<>
inline void GParameterBase::multiplyByRandom<double>(
    const double &min
    , const double &max
    , activityMode am
    , Gem::Hap::GRandomBase &gr
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleMultiplyByRandom(
            min
            , max
            , am
            , gr
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a random value in a given range
 */
template<>
inline void GParameterBase::multiplyByRandom<std::int32_t>(
    const std::int32_t &min
    , const std::int32_t &max
    , activityMode am
    , Gem::Hap::GRandomBase &gr
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32MultiplyByRandom(
            min
            , max
            , am
            , gr
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a random value in a given range. This specialization for
 * boolean values has been added for completeness and error-detection. It will throw
 * when called.
 */
template<>
inline void GParameterBase::multiplyByRandom<bool>(
    const bool &min
    , const bool &max
    , activityMode am
    , Gem::Hap::GRandomBase &gr
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        // NOTE: This will throw
        this->booleanMultiplyByRandom(
            min
            , max
            , am
            , gr
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a random value in the range [0,1[
 */
template<>
inline void GParameterBase::multiplyByRandom<float>(
    activityMode am
    , Gem::Hap::GRandomBase &gr
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatMultiplyByRandom(
            am
            , gr
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a random value in the range [0,1[
 */
template<>
inline void GParameterBase::multiplyByRandom<double>(
    activityMode am
    , Gem::Hap::GRandomBase &gr
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleMultiplyByRandom(
            am
            , gr
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a random value in the range [0,1[
 */
template<>
inline void GParameterBase::multiplyByRandom<std::int32_t>(
    activityMode am
    , Gem::Hap::GRandomBase &gr
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32MultiplyByRandom(
            am
            , gr
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a random value in the range [0,1[. This specialization for
 * boolean values has been added for completeness and error-detection. It will throw
 * when called.
 */
template<>
inline void GParameterBase::multiplyByRandom<bool>(
    activityMode am
    , Gem::Hap::GRandomBase &gr
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        // NOTE: This will throw
        this->booleanMultiplyByRandom(
            am
            , gr
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a constant value
 */
template<>
inline void GParameterBase::multiplyBy<float>(
    float val
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatMultiplyBy(
            val
            , am
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a constant value
 */
template<>
inline void GParameterBase::multiplyBy<double>(
    double val
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleMultiplyBy(
            val
            , am
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a constant value
 */
template<>
inline void GParameterBase::multiplyBy<std::int32_t>(
    std::int32_t val
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32MultiplyBy(
            val
            , am
        );
    }
}

/******************************************************************************/
/**
 * Multiplication with a constant value. This specialization for
 * boolean values has been added for completeness and error-detection.
 * It will throw when called.
 */
template<>
inline void GParameterBase::multiplyBy<bool>(
    bool val
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        // NOTE: This will throw
        this->booleanMultiplyBy(
            val
            , am
        );
    }
}

/******************************************************************************/
/**
 * Initialization with a constant value
 */
template<>
inline void GParameterBase::fixedValueInit<float>(
    float val
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatFixedValueInit(
            val
            , am
        );
    }
}

/******************************************************************************/
/**
 * Initialization with a constant value
 */
template<>
inline void GParameterBase::fixedValueInit<double>(
    double val
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleFixedValueInit(
            val
            , am
        );
    }
}

/******************************************************************************/
/**
 * Initialization with a constant value
 */
template<>
inline void GParameterBase::fixedValueInit<std::int32_t>(
    std::int32_t val
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32FixedValueInit(
            val
            , am
        );
    }
}

/******************************************************************************/
/**
 * Initialization with a constant value
 */
template<>
inline void GParameterBase::fixedValueInit<bool>(
    bool val
    , activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->booleanFixedValueInit(
            val
            , am
        );
    }
}

/******************************************************************************/
/**
 * Adds the "same-type" parameters of another GParameterBase object to this one
 */
template<>
inline void GParameterBase::add<float>(
    const std::shared_ptr<GParameterBase> &p, activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatAdd(
            p
            , am
        );
    }
}

/******************************************************************************/
/**
 * Adds the "same-type" parameters of another GParameterBase object to this one
 */
template<>
inline void GParameterBase::add<double>(
    const std::shared_ptr<GParameterBase> &p, activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleAdd(
            p
            , am
        );
    }
}

/******************************************************************************/
/**
 * Adds the "same-type" parameters of another GParameterBase object to this one
 */
template<>
inline void GParameterBase::add<std::int32_t>(
    const std::shared_ptr<GParameterBase> &p, activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32Add(
            p
            , am
        );
    }
}

/******************************************************************************/
/**
 * Adds the "same-type" parameters of another GParameterBase object to this one.
 * This specialization for boolean values has been added for completeness and error-detection.
 * It will throw when called.
 */
template<>
inline void GParameterBase::add<bool>(
    const std::shared_ptr<GParameterBase> &p, activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        // Note: This call will throw!
        this->booleanAdd(
            p
            , am
        );
    }
}

/******************************************************************************/
/**
 * Subtracts the "same-type" parameters of another GParameterBase object from this one
 */
template<>
inline void GParameterBase::subtract<float>(
    const std::shared_ptr<GParameterBase> &p, activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->floatSubtract(
            p
            , am
        );
    }
}

/******************************************************************************/
/**
 * Subtracts the "same-type" parameters of another GParameterBase object from this one
 */
template<>
inline void GParameterBase::subtract<double>(
    const std::shared_ptr<GParameterBase> &p, activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->doubleSubtract(
            p
            , am
        );
    }
}

/******************************************************************************/
/**
 * Subtracts the "same-type" parameters of another GParameterBase object from this one
 */
template<>
inline void GParameterBase::subtract<std::int32_t>(
    const std::shared_ptr<GParameterBase> &p, activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        this->int32Subtract(
            p
            , am
        );
    }
}

/******************************************************************************/
/**
 * Subtracts the "same-type" parameters of another GParameterBase object from this one.
 * This specialization for boolean values has been added for completeness and error-detection.
 * It will throw when called.
 */
template<>
inline void GParameterBase::subtract<bool>(
    const std::shared_ptr<GParameterBase> &p, activityMode am
) {
    if (this->modifiableAmMatchOrHandover(am)) {
        // NOTE: This call will throw
        this->booleanSubtract(
            p
            , am
        );
    }
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

/******************************************************************************/
/**
 * @brief Needed for Boost.Serialization
 */
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Gem::Geneva::GParameterBase)

/******************************************************************************/

