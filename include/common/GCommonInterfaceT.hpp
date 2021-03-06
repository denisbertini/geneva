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
#include <string>
#include <filesystem>
#include <fstream>
#include <memory>
#include <type_traits>

// Boost header files go here
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>

// Geneva header files go here
#include "common/GCommonEnums.hpp" // For the serialization mode
#include "common/GExceptions.hpp"
#include "common/GErrorStreamer.hpp"
#include "common/GExpectationChecksT.hpp"
#include "common/GCommonHelperFunctionsT.hpp"
#include "common/GParserBuilder.hpp"

namespace Gem {
namespace Common {
/******************************************************************************/
/**
 * This is an interface class that specifies common operations that must be
 * available for the majority of classes in the Gemfony scientific library.
 * As one example, (de-)serialization is simplified by some of the functions
 * in this class, as is the task of conversion to the derived types.
 */
template<typename g_class_type>
class GCommonInterfaceT
    // This simplifies detection of classes that implement the Gemfony interface -- see GTypeTraits.hpp
    // The problem here is that GCommonInterfaceT<g_class_type> is usually the base class of g_class_type and thus an incomplete
    // type at the time type traits are applied. Hence we use another (trivial) base class that simplifies
    // detection.
    : private gemfony_common_interface_indicator
{
public:
    /***************************************************************************/
    /**
     * Converts the class(-hierarchy) to a serial representation that is
     * then written to a stream.
     *
     * @param oarchive_stream The output stream the object should be written to
     * @param serMod The desired serialization mode
     */
    void toStream(
        std::ostream &oarchive_stream
        , const Gem::Common::serializationMode &serMod
    ) const {
        const g_class_type *local;

        // Note: (De-)serialization must happen through a pointer to the same type.
#ifdef DEBUG
        local = dynamic_cast<const g_class_type *>(this);
        if (not local) {
            throw gemfony_exception(
                g_error_streamer(
                    DO_LOG
                    , time_and_place
                )
                    << "In GCommonInterfaceT<g_class_type>::toStream(): Error!" << std::endl
                    << "Conversion failed" << std::endl
            );
        }
#else
        local = static_cast<const g_class_type *>(this);
#endif /* DEBUG */

        switch (serMod) {
            case Gem::Common::serializationMode::TEXT: {
                boost::archive::text_oarchive oa(oarchive_stream);
                oa << boost::serialization::make_nvp(
                    "classhierarchyFromT"
                    , local
                );
            } // note: explicit scope here is essential so the oa-destructor gets called

                break;

            case Gem::Common::serializationMode::XML: {
                boost::archive::xml_oarchive oa(oarchive_stream);
                oa << boost::serialization::make_nvp(
                    "classhierarchyFromT"
                    , local
                );
            } // note: explicit scope here is essential so the oa-destructor gets called

                break;

            case Gem::Common::serializationMode::BINARY: {
                boost::archive::binary_oarchive oa(oarchive_stream);
                oa << boost::serialization::make_nvp(
                    "classhierarchyFromT"
                    , local
                );
            } // note: explicit scope here is essential so the oa-destructor gets called

                break;
        }
    }

    /* ----------------------------------------------------------------------------------
     * Tested in GObject::specificTestsNoFailureExpected_GUnitTests()
     * Tested indirectly through standard tests of toString
     * ----------------------------------------------------------------------------------
     */

    /***************************************************************************/
    /**
     * Loads the object from a stream.
     *
     * @param istr The stream from which the object should be loaded
     * @param serMod The desired serialization mode
     *
     */
    void fromStream(
        std::istream &istr
        , const Gem::Common::serializationMode &serMod
    ) {
        g_class_type *local = nullptr;

        switch (serMod) {
            case Gem::Common::serializationMode::TEXT: {
                boost::archive::text_iarchive ia(istr);
                ia >> boost::serialization::make_nvp(
                    "classhierarchyFromT"
                    , local
                );
            } // note: explicit scope here is essential so the ia-destructor gets called

                break;

            case Gem::Common::serializationMode::XML: {
                boost::archive::xml_iarchive ia(istr);
                ia >> boost::serialization::make_nvp(
                    "classhierarchyFromT"
                    , local
                );
            } // note: explicit scope here is essential so the ia-destructor gets called

                break;

            case Gem::Common::serializationMode::BINARY: {
                boost::archive::binary_iarchive ia(istr);
                ia >> boost::serialization::make_nvp(
                    "classhierarchyFromT"
                    , local
                );
            } // note: explicit scope here is essential so the ia-destructor gets called

                break;
        }

        this->load_(local);
        if (local) {
            g_delete(local);
        }
    }

    /* ----------------------------------------------------------------------------------
         * Tested in GObject::specificTestsNoFailureExpected_GUnitTests()
         * Tested indirectly through standard tests of fromString
         * ----------------------------------------------------------------------------------
         */

    /***************************************************************************/
    /**
     * Converts the class to a text representation, using the currently set serialization mode for this
     * class. Note that you will have to take care yourself that serialization and de-serialization
     * happens in the same mode.
     *
     * @param serMod The desired serialization mode
     * @return A text-representation of this class (or its derivative)
     */
    std::string toString(const Gem::Common::serializationMode &serMod) const {
        std::ostringstream oarchive_stream;
        toStream(
            oarchive_stream
            , serMod
        );
        return oarchive_stream.str();
    }

    /* ----------------------------------------------------------------------------------
         * Tested in GObject::specificTestsNoFailureExpected_GUnitTests()
         * Tested as part of standard serialization tests in Geneva standard test suite
         * ----------------------------------------------------------------------------------
         */

    /***************************************************************************/
    /**
     * Initializes the object from its string representation, using the currently set serialization mode.
     * Note that the string will likely describe a derivative of g_class_type, as g_class_type cannot usually be instantiated.
     * Note also that you will have to take care yourself that serialization and de-serialization happens
     * in the same mode.
     *
     * @param descr A text representation of a g_class_type-derivative
     */
    void fromString(
        const std::string &descr
        , const Gem::Common::serializationMode &serMod
    ) {
        std::istringstream istr(descr);
        fromStream(
            istr
            , serMod
        );
    }

    /* ----------------------------------------------------------------------------------
         * Tested in GObject::specificTestsNoFailureExpected_GUnitTests()
         * Tested as part of standard serialization tests in Geneva standard test suite
         * ----------------------------------------------------------------------------------
         */

    /***************************************************************************/
    /**
     * Writes a serial representation of this object to a file. Can be used for check-pointing.
     *
     * @param p The name of the file the object should be saved to.
     * @param serMod The desired serialization mode
     */
    void toFile(
        const std::filesystem::path &p
        , const Gem::Common::serializationMode &serMod
    ) const {
        std::ofstream ofstr(
            p
            , std::ofstream::trunc
        ); // Note: will overwrite existing files

        if (not ofstr) {
            throw gemfony_exception(
                g_error_streamer(
                    DO_LOG
                    , time_and_place
                )
                    << "In GCommonInterfaceT::toFile():" << std::endl
                    << "Problems connecting to file " << p.string() << std::endl
            );
        }

        toStream(
            ofstr
            , serMod
        );
        ofstr.close();

#ifdef DEBUG
        if (not std::filesystem::exists(std::filesystem::path(p))) {
            throw gemfony_exception(
                g_error_streamer(
                    DO_LOG
                    , time_and_place
                )
                    << "In GCommonInterfaceT::toFile():" << std::endl
                    << "Data was written to " << p.string() << std::endl
                    << "but file does not seem to exist." << std::endl
            );
        }
#endif
    }

    /* ----------------------------------------------------------------------------------
         * Tested in GObject::specificTestsNoFailureExpected_GUnitTests()
         * Part of the regular Geneva standard tests for every tested object
         * ----------------------------------------------------------------------------------
         */

    /***************************************************************************/
    /**
     * Loads a serial representation of this object from file. Can be used for check-pointing.
     *
     * @param p The name of the file the object should be loaded from
     * @param serMod The desired serialization mode
     */
    void fromFile(
        const std::filesystem::path &p
        , const Gem::Common::serializationMode &serMod
    ) {
        // Check that the file exists
        if (not std::filesystem::exists(std::filesystem::path(p))) {
            throw gemfony_exception(
                g_error_streamer(
                    DO_LOG
                    , time_and_place
                )
                    << "In GCommonInterfaceT::fromFile(): Error!" << std::endl
                    << "Requested input file " << p.string() << std::endl
                    << "does not exist." << std::endl
            );
        }

        std::ifstream ifstr(p);

        if (not ifstr) {
            throw gemfony_exception(
                g_error_streamer(
                    DO_LOG
                    , time_and_place
                )
                    << "In GCommonInterfaceT::fromFile():" << std::endl
                    << "Problem connecting to file " << p.string() << std::endl
            );
        }

        fromStream(
            ifstr
            , serMod
        );
        ifstr.close();
    }

    /* ----------------------------------------------------------------------------------
         * Tested in GObject::specificTestsNoFailureExpected_GUnitTests()
         * Part of the regular Geneva standard tests for every tested object
         * ----------------------------------------------------------------------------------
         */

    /***************************************************************************/
    /**
     * Returns an XML description of the derivative it is called for
     *
     * @return An XML description of the GObject-derivative the function is called for
     */
    std::string report() const {
        return toString(Gem::Common::serializationMode::XML);
    }

    /* ----------------------------------------------------------------------------------
         * Tested in GObject::specificTestsNoFailureExpected_GUnitTests() // Check that
         * the function does return a non-empty description. Content is not checked
         * automatically.
         * ----------------------------------------------------------------------------------
         */

    /******************************************************************************/
    /**
     * Adds local configuration options to a GParserBuilder object.
     *
     * @param gpb The GParserBuilder object to which configuration options should be added
     */
    void addConfigurationOptions(
        Gem::Common::GParserBuilder &gpb
    ) {
        addConfigurationOptions_(gpb);
    }

    /******************************************************************************/
    /**
     * Writes a configuration file to disk
     *
     * @param configFile The name of the configuration file to be written
     * @param header A header to be prepended to the configuration file
     */
    void writeConfigFile(
        std::filesystem::path const &configFile
        , const std::string &header
    ) {
        // This class will handle the interaction with configuration files
        Gem::Common::GParserBuilder gpb;

        // Recursively add configuration options to gpb,
        // starting with the most derived class
        addConfigurationOptions(gpb);

        // Write out the configuration file
        gpb.writeConfigFile(
            configFile
            , header
            , true
        );
    }

    /******************************************************************************/
    /**
     * Reads a configuration file from disk
     *
     * @param configFile The name of the configuration file to be parsed
     */
    void readConfigFile(std::filesystem::path const &configFile) {
        // This class will handle the interaction with configuration files
        Gem::Common::GParserBuilder gpb;

        // Recursively add configuration options to gpb,
        // starting with the most derived class
        addConfigurationOptions(gpb);

        // Read in the configuration file
        gpb.parseConfigFile(configFile);
    }

    /***************************************************************************/
    /**
     * Emits a name for this class / object. Wrapper to avoid public virtual.
     */
    std::string name() const { return this->name_(); }

    /***************************************************************************/
    /**
     * Checks for compliance with expectations with respect to another object
     * of type g_class_type. This purely virtual function ensures the well-formedness of the
     * compare hierarchy in derived classes.
     *
     * @param cp A constant reference to another object of the same type, camouflaged as a base object
     * @param e The expected outcome of the comparison
     * @param limit The maximum deviation for floating point values (important for similarity checks)
     */
    void compare(
        const g_class_type &cp // the other object
        , const Gem::Common::expectation &e // the expectation for this object, e.g. equality
        , const double &limit // the limit for allowed deviations of floating point types
    ) const {
        this->compare_(
            cp
            , e
            , limit
        );
    }


    /***************************************************************************/
    /**
     * Creates a clone of this object, storing it in a std::shared_ptr<g_class_type>
     */
    std::shared_ptr<g_class_type> clone() const {
        return std::shared_ptr<g_class_type>(clone_());
    }

    /***************************************************************************/
    /**
     * The function creates a clone of the g_class_type pointer, converts it to a pointer to a derived class
     * and emits it as a std::shared_ptr<> . Note that this template will only be accessible to the
     * compiler if g_class_type is a base type of clone_type.
     *
     * @return A converted clone of this object, wrapped into a std::shared_ptr
     */
    template<typename clone_type>
    std::shared_ptr<clone_type> clone(
        typename std::enable_if<std::is_base_of<g_class_type, clone_type>::value>::type *dummy = nullptr
    ) const {
        return Gem::Common::convertSmartPointer<g_class_type, clone_type>(std::shared_ptr<g_class_type>(this->clone_()));
    }

    /***************************************************************************/
    /**
     * Loads the data of another g_class_type(-derivative), wrapped in a shared pointer. Note that this
     * function is only accessible to the compiler if load_type is a derivative of g_class_type.
     *
     * @param cp A copy of another g_class_type-derivative, wrapped into a std::shared_ptr<>
     */
    template<typename load_type>
    void load(
        const std::shared_ptr<load_type> &cp
        , typename std::enable_if<std::is_base_of<g_class_type, load_type>::value>::type *dummy = nullptr
    ) {
        load_(cp.get());
    }

    /***************************************************************************/
    /**
     * Loads the data of another g_class_type(-derivative), presented as a constant reference. Note that this
     * function is only accessible to the compiler if load_type is a derivative of g_class_type.
     *
     * @param cp A copy of another g_class_type-derivative, wrapped into a std::shared_ptr<>
     */
    template<typename load_type>
    void load(
        const load_type &cp
        , typename std::enable_if<std::is_base_of<g_class_type, load_type>::value>::type *dummy = nullptr
    ) {
        load_(&cp);
    }

    /***************************************************************************/
    /**
     * Applies modifications to this object. This is needed for testing purposes
     */
    bool modify_GUnitTests() {
        return this->modify_GUnitTests_();
    }

    /***************************************************************************/
    /**
     * Performs self tests that are expected to succeed. This is needed for testing purposes
     */
    void specificTestsNoFailureExpected_GUnitTests() {
        this->specificTestsNoFailureExpected_GUnitTests_();
    }

    /***************************************************************************/
    /**
     * Performs self tests that are expected to fail. This is needed for testing purposes
     */
    void specificTestsFailuresExpected_GUnitTests() {
        this->specificTestsFailuresExpected_GUnitTests_();
    }

protected:
    /***************************************************************************/
    // Defaulted constructors -- rule of five

    GCommonInterfaceT() = default;
    GCommonInterfaceT(const GCommonInterfaceT<g_class_type> &cp) = default;
    GCommonInterfaceT(GCommonInterfaceT<g_class_type> && cp) = default;
    ~GCommonInterfaceT() = default;

    GCommonInterfaceT<g_class_type>& operator=(GCommonInterfaceT<g_class_type> const &) = default;
    GCommonInterfaceT<g_class_type>& operator=(GCommonInterfaceT<g_class_type> &&) = default;

    /***************************************************************************/
    /** @brief Loads the data of another g_class_type */
    virtual G_API_COMMON void load_(const g_class_type *) BASE = 0;

    /***************************************************************************/
    /** @brief Allow access to this classes compare_ function */
    friend void compare_base_t<GCommonInterfaceT<g_class_type>>(
        GCommonInterfaceT<g_class_type> const &
        , GCommonInterfaceT<g_class_type> const &
        , GToken &
    );

    /***************************************************************************/
    /**
     * Checks for compliance with expectations with respect to another object
     * of type g_class_type. This purely virtual function ensures the well-formedness of the
     * compare hierarchy in derived classes.
     *
     * @param cp A constant reference to another object of the same type, camouflaged as a base object
     * @param e The expected outcome of the comparison
     * @param limit The maximum deviation for floating point values (important for similarity checks)
     */
    virtual void compare_(
        const g_class_type &cp // the other object
        , const Gem::Common::expectation &e // the expectation for this object, e.g. equality
        , const double &limit // the limit for allowed deviations of floating point types
    ) const BASE = 0;

    /***************************************************************************/
    /**
     * Checks for compliance with expectations with respect to another object
     * of the same type. This function does the real check. Without it we would get
     * an error about "no known conversion from GCommonInterfaceT<g_class_type> to g_class_type.
     *
     * @param cp A constant reference to another object of the same type, camouflaged as a base object
     * @param e The expected outcome of the comparison
     * @param limit The maximum deviation for floating point values (important for similarity checks)
     */
    void compare_(
        const GCommonInterfaceT<g_class_type> &cp // the other object
        , const Gem::Common::expectation &e // the expectation for this object, e.g. equality
        , const double &limit // the limit for allowed deviations of floating point types
    ) const {
        using namespace Gem::Common;

        // Check that cp isn't the same object as this one
        ptrDifferenceCheck(
            &cp
            , this
        );

        // No parent classes to check...

        // ... and no local data

        // We consider two instances of this class to be always equal, as they
        // do not have any local data and this is the base class. Hence
        // we throw an expectation violation for the expectation INEQUALITY.
        if (expectation::INEQUALITY == e) {
            throw g_expectation_violation(
                "In GCommonInterfaceT<g_class_type>: instance is empty and a base class, hence the expectation of inequality is always violated."
            );
        }
    }

    /******************************************************************************/
    /**
     * Adds local configuration options to a GParserBuilder object. This is a protected, virtual version
     * of this function that is overloaded in derived classes.
     *
     * @param gpb The GParserBuilder object to which configuration options should be added
     */
    virtual void addConfigurationOptions_(
        Gem::Common::GParserBuilder &gpb
    ) BASE {
        // No local data, no relevant parent classes, hence nothing to do
    }

    /***************************************************************************/
    /** @brief Applies modifications to this object. This is needed for testing purposes */
    virtual G_API_GENEVA bool modify_GUnitTests_() BASE = 0;
    /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
    virtual G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests_() BASE = 0;
    /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
    virtual G_API_GENEVA void specificTestsFailuresExpected_GUnitTests_() BASE = 0;

private:
    /***************************************************************************/
    /**
     * Emits a name for this class / object
     */
    virtual std::string name_() const BASE {
        return std::string("GCommonInterfaceT<g_class_type>");
    }

    /***************************************************************************/
    /** @brief Creates a deep clone of this object */
    virtual G_API_COMMON g_class_type *clone_() const BASE = 0;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
