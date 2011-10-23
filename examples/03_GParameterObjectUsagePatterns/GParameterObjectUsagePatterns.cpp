/**
 * @file GParameterObjectUsagePatterns.cpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
 *
 * This file is part of the Geneva library collection.
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is free software: you can redistribute and/or modify it under
 * the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Geneva library. If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.com .
 */

// Standard header files go here
#include <iostream>

// Boost header files go here
#include <boost/shared_ptr.hpp>

// Geneva header files go here
#include "geneva/Go2.hpp" // Includes all of the parameter object types

// All GRandom-related code is in the namespace Gem::Hap
using namespace Gem::Geneva;

int main(int argc, char **argv) {
	{ // Usage patterns for the GDoubleObject class
		std::cout << "GDoubleObject:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GDoubleObject o1; // Default construction
		GDoubleObject o2(o1); // Copy construction
		GDoubleObject o3(2.); // Initialization by value
		GDoubleObject o4(0.,2.); // Random initialization in a given range
		boost::shared_ptr<GDoubleObject> p(new GDoubleObject(0.,2.)); // Construction and access frequently happens through smart pointers

		//-----------------------------------------------------
		// Assignment, value setting and retrieval
		o1 = 1.; // Assigning and setting a value
		o2.setValue(2.);
		o4 = o1; // Assignment of another object
		std::cout << o4.value() << std::endl; // Value retrieval

		//-----------------------------------------------------
		// Boundaries
		std::cout << o4.getLowerInitBoundary() << std::endl; // Retrieval of lower init boundary
		std::cout << o4.getUpperInitBoundary() << std::endl; // Retrieval of upper init boundary

		//-----------------------------------------------------
		// Assignment of an adaptor
		double sigma = 0.1; // "step width" of gauss mutation
		double sigmaSigma = 0.8; // adaption of sigma
		double minSigma = 0., maxSigma = 0.5; // allowed value range of sigma
		double adProb = 0.05; // 5% probability for the adaption of this object when adaptor is called
		boost::shared_ptr<GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(sigma, sigmaSigma, minSigma, maxSigma));
		gdga_ptr->setAdaptionProbability(adProb);
		p->addAdaptor(gdga_ptr);
	}

	{ // Usage patterns for the GConstrainedDoubleObject class
		std::cout << "GConstrainedDoubleObject:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GConstrainedDoubleObject o1; // Default construction
		GConstrainedDoubleObject o2(o1); // Copy construction
		GConstrainedDoubleObject o3(2.); // Initialization by value
		GConstrainedDoubleObject o4(0.,2.); // Initialization of value boundaries
		GConstrainedDoubleObject o5(1.,0.,2.); // Initialization with value and value boundaries
		boost::shared_ptr<GConstrainedDoubleObject> p(new GConstrainedDoubleObject(0.,2.)); // Construction and access frequently happens through smart pointers

		//-----------------------------------------------------
		// Assignment, value setting and retrieval
		o1 = 1.; // Assigning a value
		o2.setValue(1.5);
		o5 = o1; // Assignment of another object
		std::cout << o4.value() << " " << o5.value() << std::endl; // Value retrieval

		//-----------------------------------------------------
		// Boundaries
		std::cout << o4.getLowerBoundary() << std::endl; // Retrieval of lower value boundary
		std::cout << o4.getUpperBoundary() << std::endl; // Retrieval of upper value boundary

		//-----------------------------------------------------
		// Assignment of an adaptor (same as for GDoubleObject)
		double sigma = 0.1; // "step width" of gauss mutation
		double sigmaSigma = 0.8; // adaption of sigma
		double minSigma = 0., maxSigma = 0.5; // allowed value range of sigma
		double adProb = 0.05; // 5% probability for the adaption of this object when adaptor is called
		boost::shared_ptr<GDoubleGaussAdaptor> gdga_ptr(new GDoubleGaussAdaptor(sigma, sigmaSigma, minSigma, maxSigma));
		gdga_ptr->setAdaptionProbability(adProb);
		p->addAdaptor(gdga_ptr);
	}

	{ // Usage patterns for the GDoubleObjectCollection class
		std::cout << "GDoubleObjectCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GDoubleObjectCollection c1; // Default constructor
		GDoubleObjectCollection c2(c1); // Copy construction
		boost::shared_ptr<GDoubleObjectCollection> p_c3(new GDoubleObjectCollection(c1)); // Copy construction inside of smart pointer
		// Note: Copy construction will create deep copies
		// of all objects stored in c1

		//-----------------------------------------------------
		// Filling with objects
		for(std::size_t i=0; i<10; i++) {
			// Create a smart pointer wrapping a GDoubleObject
			boost::shared_ptr<GDoubleObject> p(new GDoubleObject());
			// Configure GDoubleObject as required. E.g., add adaptors
			// ...
			// Add to the collection
			c1.push_back(p);
		}

		// Note: No adaptor is added to the collection itself, only
		// to the objects contained in it.

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will create
		// deep copies of all objects stored in c1
		c2 = c1;
		*p_c3 = c1;
		//-----------------------------------------------------
		// Access to parameter objects in the collection
		for(std::size_t i=0; i<10; i++) {
			std::cout << p_c3->at(i)->value() << std::endl;
			std::cout << c1[i]->value() << std::endl;
		}

		// Note: The iterator points to a smart pointer, so in order to
		// call a function on the parameter objects we first need to
		// dereference the iterator, then the smart pointer
		GDoubleObjectCollection::iterator it;
		for(it=c1.begin(); it!=c1.end(); ++it) {
			std::cout << (*it)->value() << std::endl;
		}
	}

	{ // Usage patterns for the GConstrainedDoubleObjectCollection class
		std::cout << "GConstrainedDoubleObjectCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GConstrainedDoubleObjectCollection c1; // Default constructor
		GConstrainedDoubleObjectCollection c2(c1); // Copy construction
		boost::shared_ptr<GConstrainedDoubleObjectCollection> p_c3(new GConstrainedDoubleObjectCollection(c1)); // Copy construction inside of smart pointer
		// Note: Copy construction will create deep copies
		// of all objects stored in c1

		//-----------------------------------------------------
		// Filling with objects
		for(std::size_t i=0; i<10; i++) {
			// Create a smart pointer wrapping a GDoubleObject
			boost::shared_ptr<GConstrainedDoubleObject> p(new GConstrainedDoubleObject());
			// Configure GConstrainedDoubleObject as required. E.g., add adaptors
			// ...
			// Add to the collection
			c1.push_back(p);
		}

		// Note: No adaptor is added to the collection itself, only
		// to the objects contained in it.

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will create
		// deep copies of all objects stored in c1
		c2 = c1;
		*p_c3 = c1;

		//-----------------------------------------------------
		// Access to parameter objects in the collection
		for(std::size_t i=0; i<10; i++) {
			std::cout << p_c3->at(i)->value() << std::endl;
			std::cout << c1[i]->value() << std::endl;
		}

		// Note: The iterator points to a smart pointer, so in order to
		// call a function on the parameter objects we first need to
		// dereference the iterator, then the smart pointer
		GConstrainedDoubleObjectCollection::iterator it;
		for(it=c1.begin(); it!=c1.end(); ++it) {
			std::cout << (*it)->value() << std::endl;
		}
	}

	{ // Usage patterns for the GDoubleCollection class
		std::cout << "GDoubleCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GDoubleCollection c1; // Default construction
		GDoubleCollection c2(c1); // Copy construction
		// Copy construction inside of smart pointer
		boost::shared_ptr<GDoubleCollection> p_c3(new GDoubleCollection(c1));
		// 100 double values, randomly initialized in the range [-3.,3[
		GDoubleCollection c4(100, -3., 3.);

		//-----------------------------------------------------
		// Filling with objects
		for(double d=0.; d<100.; d+=1.) {
			c1.push_back(d);
		}

		//-----------------------------------------------------
		// Adding an adaptor
		double sigma = 0.1; // "step width" of gauss mutation
		double sigmaSigma = 0.8; // adaption of sigma
		double minSigma = 0., maxSigma = 0.5; // allowed value range of sigma
		// 5% probability for the adaption of this object when adaptor is called
		double adProb = 0.05;
		boost::shared_ptr<GDoubleGaussAdaptor> gdga_ptr(
				new GDoubleGaussAdaptor(
						sigma, sigmaSigma, minSigma, maxSigma
				)
	    );
		gdga_ptr->setAdaptionProbability(adProb);
		c1.addAdaptor(gdga_ptr);

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will also create
		// deep copies of the adaptor
		c2=c1;
		*p_c3 = c1;

		//-----------------------------------------------------
		// Access to parameter objects in the collection
		for(std::size_t i=0; i<c1.size(); i++) {
			std::cout << c1[i] << std::endl;
			std::cout << c1.at(i) << std::endl;
		}
		GDoubleCollection::iterator it;
		for(it=c1.begin(); it!=c1.end(); ++it) {
			std::cout << *it << std::endl;
		}
		//-----------------------------------------------------
	}

	{ // Usage patterns for the GConstrainedDoubleCollection class
		std::cout << "GConstrainedDoubleCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		// Initialization with 100 variables and constraint [-10, 10[
		GConstrainedDoubleCollection c1(100, -10, 200.);
		GConstrainedDoubleCollection c2(c1); // Copy construction

		// Note -- we do not currently fill in additional data items. This
		// class is not yet at its final stage.

		//-----------------------------------------------------
		// Adding an adaptor
		double sigma = 0.1; // "step width" of gauss mutation
		double sigmaSigma = 0.8; // adaption of sigma
		double minSigma = 0., maxSigma = 0.5; // allowed value range of sigma
		// 5% probability for the adaption of this object when adaptor is called
		double adProb = 0.05;
		boost::shared_ptr<GDoubleGaussAdaptor> gdga_ptr(
				new GDoubleGaussAdaptor(
						sigma, sigmaSigma, minSigma, maxSigma
				)
	    );
		gdga_ptr->setAdaptionProbability(adProb);
		c1.addAdaptor(gdga_ptr);

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will also create
		// deep copies of the adaptor
		c2 = c1;

		//-----------------------------------------------------
		// Access to parameter objects in the collection
		// Note: We currently recommend not to use the subscript and at()
		// operators or iterators
		for(std::size_t i=0; i<c1.size(); i++) {
			c1.setValue(i, double(i));
			std::cout << c1.value(i) << std::endl;
		}
		//-----------------------------------------------------
	}

	{ // Usage patterns for the GInt32Object class
		std::cout << "GInt32Object:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GInt32Object o1; // Default construction
		GInt32Object o2(o1); // Copy construction
		GInt32Object o3(2); // Initialization by value
		GInt32Object o4(0,2); // Random initialization in a given range
		boost::shared_ptr<GInt32Object> p_o5(new GInt32Object(0,2)); // Construction and access frequently happens through smart pointers

		//-----------------------------------------------------
		// Assignment, value setting and retrieval
		o1 = 1; // Assigning and setting a value
		o2.setValue(2);
		o4 = o1; // Assignment of another object
		std::cout << o4.value() << std::endl; // Value retrieval

		//-----------------------------------------------------
		// Boundaries
		std::cout << o4.getLowerInitBoundary() << std::endl; // Retrieval of lower init boundary
		std::cout << o4.getUpperInitBoundary() << std::endl; // Retrieval of upper init boundary

		//-----------------------------------------------------
		// Assignment of an adaptor
		boost::shared_ptr<GInt32FlipAdaptor> ifa_ptr(new GInt32FlipAdaptor());
		ifa_ptr->setAdaptionProbability(0.05); // 5% probability
		p_o5->addAdaptor(ifa_ptr);
	}

	{ // Usage patterns for the GConstrainedInt32Object class
		std::cout << "GConstrainedInt32Object:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GConstrainedInt32Object o1; // Default construction
		GConstrainedInt32Object o2(o1); // Copy construction
		GConstrainedInt32Object o3(2); // Initialization by value
		GConstrainedInt32Object o4(0,10); // Initialization of allowed initialization range
		GConstrainedInt32Object o5(1,0,10); // Initialization with value and allowed initialization range
		boost::shared_ptr<GConstrainedInt32Object> p_o6(new GConstrainedInt32Object(0,2)); // Construction and access frequently happens through smart pointers

		//-----------------------------------------------------
		// Assignment, value setting and retrieval
		o1 = 1; // Assigning and setting a value
		o2.setValue(2);
		o4 = o1; // Assignment of another object
		std::cout << o4.value() << std::endl; // Value retrieval

		//-----------------------------------------------------
		// Boundaries
		std::cout << o4.getLowerBoundary() << std::endl; // Retrieval of lower init boundary
		std::cout << o4.getUpperBoundary() << std::endl; // Retrieval of upper init boundary

		//-----------------------------------------------------
		// Assignment of an adaptor
		boost::shared_ptr<GInt32FlipAdaptor> ifa_ptr(new GInt32FlipAdaptor());
		ifa_ptr->setAdaptionProbability(0.05); // 5% probability
		p_o6->addAdaptor(ifa_ptr);
	}

	{ // Usage patterns for the GInt32ObjectCollection class
		std::cout << "GInt32ObjectCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GInt32ObjectCollection c1; // Default constructor
		GInt32ObjectCollection c2(c1); // Copy construction
		// Copy construction inside of smart pointer
		boost::shared_ptr<GInt32ObjectCollection> p_c3(new GInt32ObjectCollection(c1));
		// Note: Copy construction will create deep copies
		// of all objects stored in c1

		//-----------------------------------------------------
		// Filling with objects
		for(std::size_t i=0; i<10; i++) {
			// Create a smart pointer wrapping a GInt32Object
			boost::shared_ptr<GInt32Object> p(new GInt32Object());
			// Configure GInt32Object as required. E.g., add adaptors
			// ...
			// Add to the collection
			c1.push_back(p);
		}

		// Note: No adaptor is added to the collection itself, only
		// to the objects contained in it.

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will create
		// deep copies of all objects stored in c1
		c2 = c1;
		*p_c3 = c1;

		//-----------------------------------------------------
		// Access to parameter objects in the collection
		for(std::size_t i=0; i<10; i++) {
			std::cout << p_c3->at(i)->value() << std::endl;
			std::cout << c1[i]->value() << std::endl;
		}

		// Note: The iterator points to a smart pointer, so in order to
		// call a function on the parameter objects we first need to
		// dereference the iterator, then the smart pointer
		GInt32ObjectCollection::iterator it;
		for(it=c1.begin(); it!=c1.end(); ++it) {
			std::cout << (*it)->value() << std::endl;
		}
	}

	{ // Usage patterns for the GConstrainedInt32ObjectCollection class
		std::cout << "GConstrainedInt32ObjectCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GConstrainedInt32ObjectCollection c1; // Default constructor
		GConstrainedInt32ObjectCollection c2(c1); // Copy construction
		// Copy construction inside of smart pointer
		boost::shared_ptr<GConstrainedInt32ObjectCollection>
		    p_c3(new GConstrainedInt32ObjectCollection(c1));
		// Note: Copy construction will create deep copies
		// of all objects stored in c1

		//-----------------------------------------------------
		// Filling with objects
		for(std::size_t i=0; i<10; i++) {
			// Create a smart pointer wrapping a GConstrainedInt32Object
			boost::shared_ptr<GConstrainedInt32Object> p(new GConstrainedInt32Object());
			// Configure GConstrainedInt32Object as required. E.g., add adaptors
			// ...
			// Add to the collection
			c1.push_back(p);
		}

		// Note: No adaptor is added to the collection itself, only
		// to the objects contained in it.

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will create
		// deep copies of all objects stored in c1
		c2 = c1;
		*p_c3 = c1;

		//-----------------------------------------------------
		// Access to parameter objects in the collection
		for(std::size_t i=0; i<10; i++) {
			std::cout << p_c3->at(i)->value() << std::endl;
			std::cout << c1[i]->value() << std::endl;
		}

		// Note: The iterator points to a smart pointer, so in order to
		// call a function on the parameter objects we first need to
		// dereference the iterator, then the smart pointer
		GConstrainedInt32ObjectCollection::iterator it;
		for(it=c1.begin(); it!=c1.end(); ++it) {
			std::cout << (*it)->value() << std::endl;
		}
	}

	{ // Usage patterns for the GInt32Collection class
		std::cout << "GInt32Collection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GInt32Collection c1; // Default construction
		GInt32Collection c2(c1); // Copy construction
		// Copy construction inside of smart pointer
		boost::shared_ptr<GInt32Collection> p_c3(new GInt32Collection(c1));
		// 100 boost::int32_t values, with an initialization range of [-3,3]
		GInt32Collection c4(100, -3, 3);

		//-----------------------------------------------------
		// Filling with data
		for(boost::int32_t i=0.; i<100; i++) {
			c1.push_back(i);
		}

		//-----------------------------------------------------
		// Adding an adaptor
		boost::shared_ptr<GInt32FlipAdaptor> ifa_ptr(new GInt32FlipAdaptor());
		ifa_ptr->setAdaptionProbability(0.05); // 5% probability
		c1.addAdaptor(ifa_ptr);

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will also create
		// deep copies of the adaptor
		c2=c1;
		*p_c3 = c1;

		//-----------------------------------------------------
		// Access to parameter objects in the collection
		for(std::size_t i=0; i<c1.size(); i++) {
			std::cout << c1[i] << std::endl;
			std::cout << c1.at(i) << std::endl;
		}
		GInt32Collection::iterator it;
		for(it=c1.begin(); it!=c1.end(); ++it) {
			std::cout << *it << std::endl;
		}
		//-----------------------------------------------------
	}

	{ // Usage patterns for the GBooleanObject class
		std::cout << "GBooleanObject:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GBooleanObject o1; // Default construction
		GBooleanObject o2(o1); // Copy construction
		GBooleanObject o3(true); // Initialization by value
		// Construction and access frequently happens through smart pointers
		boost::shared_ptr<GBooleanObject> p(new GBooleanObject(true));

		//-----------------------------------------------------
		// Assignment, value setting and retrieval
		o1 = false; // Assigning and setting a value
		o2.setValue(false);
		o3 = o1; // Assignment of another object
		// Value retrieval and value emission
		std::cout << (o3.value()?true:false) << std::endl;

		//-----------------------------------------------------
		// Assignment of an adaptor
		boost::shared_ptr<GBooleanAdaptor> bad_ptr(new GBooleanAdaptor());
		bad_ptr->setAdaptionProbability(0.05); // 5% adaption probability
		p->addAdaptor(bad_ptr);
	}

	{ // Usage patterns for the GBooleanObjectCollection class
		std::cout << "GBooleanObjectCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GBooleanObjectCollection c1; // Default constructor
		GBooleanObjectCollection c2(c1); // Copy construction
		boost::shared_ptr<GBooleanObjectCollection> p_c3(
				new GBooleanObjectCollection(c1)
		); // Copy construction inside of smart pointer
		// Note: Copy construction will create deep copies
		// of all objects stored in c1

		//-----------------------------------------------------
		// Filling with objects
		for(std::size_t i=0; i<10; i++) {
			// Create a smart pointer wrapping a GBooleanObject
			boost::shared_ptr<GBooleanObject> p(new GBooleanObject());
			// Configure GBooleanObject as required. E.g., add adaptors
			// ...
			// Add to the collection
			c1.push_back(p);
		}

		// Note: No adaptor is added to the collection itself, only
		// to the objects contained in it.

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will create
		// deep copies of all objects stored in c1
		c2 = c1;
		*p_c3 = c1;
		//-----------------------------------------------------
		// Access to parameter objects in the collection
		for(std::size_t i=0; i<10; i++) {
			std::cout << p_c3->at(i)->value() << std::endl;
			std::cout << c1[i]->value() << std::endl;
		}

		// Note: The iterator points to a smart pointer, so in order to
		// call a function on the parameter objects we first need to
		// dereference the iterator, then the smart pointer
		GBooleanObjectCollection::iterator it;
		for(it=c1.begin(); it!=c1.end(); ++it) {
			std::cout << (*it)->value() << std::endl;
		}
	}

	{ // Usage patterns for the GBooleanCollection class
		std::cout << "GBooleanCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GBooleanCollection c1; // Default construction
		GBooleanCollection c2(c1); // Copy construction
		GBooleanCollection c3(100); // Initialization with 100 random booleans
		// Initialization with 100 random booleans, of which 25% have a true value
		GBooleanCollection c4(100, 0.25);
		// Copy construction inside of smart pointer
		boost::shared_ptr<GBooleanCollection> p_c5(new GBooleanCollection(c1));

		//-----------------------------------------------------
		// Filling with data
		for(std::size_t i=0; i<100; i++) {
			c1.push_back(i%2==0?true:false);
		}

		//-----------------------------------------------------
		// Adding an adaptor
		boost::shared_ptr<GBooleanAdaptor> bad_ptr(new GBooleanAdaptor());
		bad_ptr->setAdaptionProbability(0.05); // 5% adaption probability
		p_c5->addAdaptor(bad_ptr);

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will also create
		// deep copies of the adaptor
		c2=c1;
		*p_c5 = c1;

		//-----------------------------------------------------
		// Access to parameter objects in the collection
		for(std::size_t i=0; i<c1.size(); i++) {
			std::cout << (c1[i]?"true":"false") << std::endl;
			std::cout << (c1.at(i)?"true":"false") << std::endl;
		}
		GBooleanCollection::iterator it;
		for(it=c1.begin(); it!=c1.end(); ++it) {
			std::cout << (*it?"true":"false") << std::endl;
		}
		//-----------------------------------------------------
	}

	{ // Usage patterns for the GParameterObjectCollection class
		std::cout << "GParameterObjectCollection:" << std::endl;

		//-----------------------------------------------------
		// Construction
		GParameterObjectCollection c1; // Default constructor
		GParameterObjectCollection c2(c1); // Copy construction
		boost::shared_ptr<GParameterObjectCollection> p_c3(
				new GParameterObjectCollection(c1)
		); // Copy construction inside of smart pointer
		// Note: Copy construction will create deep copies
		// of all objects stored in c1

		//-----------------------------------------------------
		// Filling with objects. Note that they may have
		// different types, but must all be derived from
		// GParameterBase

		// Create a smart pointer wrapping a GDoubleObject
		boost::shared_ptr<GDoubleObject> p_d(new GDoubleObject());
		// Configure GDoubleObject as required. E.g., add adaptors
		// ...
		// Add to the collection
		c1.push_back(p_d);

		// Create a smart pointer wrapping a GInt32Object
		boost::shared_ptr<GInt32Object> p_i(new GInt32Object());
		// Configure GInt32Object as required. E.g., add adaptors
		// ...
		// Add to the collection
		c1.push_back(p_i);

		// Create another GParameterObjectCollection object.
		// As it is derived from GParameterBase, we can store it
		// in GParameterObjectCollection objects and create
		// tree-like structures in this way
		boost::shared_ptr<GParameterObjectCollection> p_child(
				new GParameterObjectCollection()
		);
		c1.push_back(p_child);

		// Note: No adaptor is added to the collection itself, only
		// to the objects contained in it (if they support this).

		//-----------------------------------------------------
		// Assignment through operator= . Note: This will create
		// deep copies of all objects stored in c1
		c2 = c1;
		*p_c3 = c1;

		//-----------------------------------------------------
		// Access to parameter objects in the collection

		// Direct conversion, if we know the target type
		boost::shared_ptr<GDoubleObject> p_d2 = c1.at<GDoubleObject>(0);

		// Conversion iterator -- will return all GDoubleObject items
		// stored on this level. Note that the conversion iterator will
		// *not* recurse into p_child .
		GParameterObjectCollection::conversion_iterator<GDoubleObject> it_conv(c1.end());
		for(it_conv=c1.begin(); it_conv!=c1.end(); ++it_conv) {
			boost::shared_ptr<GDoubleObject> p_conv = *it_conv;
			std::cout << p_conv->value() << std::endl;
		}
		//-----------------------------------------------------
	}

	return 0;
}
