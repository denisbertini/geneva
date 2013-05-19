/**
 * @file GPlotDesigner.cpp
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

#include "common/GPlotDesigner.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor
 */
GBasePlotter::GBasePlotter()
	: drawingArguments_("")
	, x_axis_label_("x")
	, y_axis_label_("y")
	, z_axis_label_("z")
	, plot_label_("")
	, dsMarker_("")
	, id_(0)
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 *
 * @param cp A copy of another GBasePlotter object
 */
GBasePlotter::GBasePlotter(const GBasePlotter& cp)
	: drawingArguments_(cp.drawingArguments_)
	, x_axis_label_(cp.x_axis_label_)
	, y_axis_label_(cp.y_axis_label_)
	, z_axis_label_(cp.z_axis_label_)
	, plot_label_(cp.plot_label_)
	, dsMarker_(cp.dsMarker_)
	, id_(cp.id_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GBasePlotter::~GBasePlotter()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 */
void GBasePlotter::operator=(const GBasePlotter& cp) {
	drawingArguments_ = cp.drawingArguments_;
	x_axis_label_ = cp.x_axis_label_;
	y_axis_label_ = cp.y_axis_label_;
	z_axis_label_ = cp.z_axis_label_;
	plot_label_   = cp.plot_label_;
	dsMarker_     = cp.dsMarker_;
	id_           = cp.id_;
}

/******************************************************************************/
/**
 * Allows to set the drawing arguments for this plot
 *
 * @param drawingArguments The drawing arguments for this plot
 */
void GBasePlotter::setDrawingArguments(std::string drawingArguments) {
	drawingArguments_ = drawingArguments;
}

/******************************************************************************/
/**
 * Retrieve the current drawing arguments
 */
std::string GBasePlotter::drawingArguments() const {
	return drawingArguments_;
}

/******************************************************************************/
/**
 * Sets the label for the x-axis
 * */
void GBasePlotter::setXAxisLabel(std::string x_axis_label) {
	x_axis_label_ = x_axis_label;
}

/******************************************************************************/
/**
 * Retrieve the x-axis label
 */
std::string GBasePlotter::xAxisLabel() const {
	return x_axis_label_;
}

/******************************************************************************/
/**
 * Sets the label for the y-axis
 */
void GBasePlotter::setYAxisLabel(std::string y_axis_label) {
	y_axis_label_ = y_axis_label;
}

/******************************************************************************/
/**
 * Retrieve the y-axis label
 */
std::string GBasePlotter::yAxisLabel() const {
	return y_axis_label_;
}

/******************************************************************************/
/**
 * Sets the label for the z-axis
 */
void GBasePlotter::setZAxisLabel(std::string z_axis_label) {
	z_axis_label_ = z_axis_label;
}

/******************************************************************************/
/**
 * Retrieve the z-axis label
 */
std::string GBasePlotter::zAxisLabel() const {
	return z_axis_label_;
}

/******************************************************************************/
/**
 * Allows to assign a label to the entire plot
 *
 * @param pL A label to be assigned to the entire plot
 */
void GBasePlotter::setPlotLabel(std::string pL) {
	plot_label_ = pL;
}

/******************************************************************************/
/**
 * Allows to retrieve the plot label
 *
 * @return The label that has been assigned to the plot
 */
std::string GBasePlotter::plotLabel() const {
	return plot_label_;
}

/******************************************************************************/
/**
 * Allows to assign a marker to data structures in the output file
 *
 * @param A marker that has been assigned to the output data structures
 */
void GBasePlotter::setDataStructureMarker(std::string dsMarker) {
	dsMarker_ = dsMarker;
}

/******************************************************************************/
/**
 * Allows to retrieve the data structure marker
 *
 * @return The marker that has been assigned to the output data structures
 */
std::string GBasePlotter::dsMarker() const {
	return dsMarker_;
}

/******************************************************************************/
/**
 * Allows to retrieve the id of this object
 */
std::size_t GBasePlotter::id() const {
	return id_;
}

/******************************************************************************/
/**
 * Sets the id of the object
 *
 * @param id The id to be assigned to this object
 */
void GBasePlotter::setId(const std::size_t& id) {
	id_ = id;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Specialization of projectX for <x_type, y_type> = <double, double>, that will return a
 * GHistogram1D object, wrapped into a boost::shared_ptr<GHistogram1D>. In case of a
 * default-constructed range, the function will attempt to determine suitable parameters
 * for the range settings.
 *
 * @param nBins The number of bins of the histogram
 * @param range The minimum and maximum boundaries of the histogram
 */
template<>
boost::shared_ptr<GDataCollector1T<double> >
GDataCollector2T<double, double>::projectX(std::size_t nBinsX, boost::tuple<double, double> rangeX) const {
	boost::tuple<double, double> myRangeX;
	if(rangeX == boost::tuple<double, double>()) {
		// Find out about the minimum and maximum values in the data_ array
		boost::tuple<double,double,double,double> extremes = Gem::Common::getMinMax(this->data_);
		myRangeX = boost::tuple<double,double>(boost::get<0>(extremes), boost::get<1>(extremes));
	} else {
		myRangeX = rangeX;
	}

	// Construct the result object
	boost::shared_ptr<GHistogram1D> result(new GHistogram1D(nBinsX, myRangeX));
	result->setXAxisLabel(this->xAxisLabel());
	result->setYAxisLabel("Number of entries");
	result->setPlotLabel(this->plotLabel() + " / x-projection");

	// Add data to the object
	for(std::size_t i=0; i<data_.size(); i++) {
		(*result) & boost::get<0>(data_.at(i));
	}

	// Return the data
	return result;
}

/******************************************************************************/
/**
 * Specialization of projectY for <x_type, y_type> = <double, double>, that will return a
 * GHistogram1D object, wrapped into a boost::shared_ptr<GHistogram1D>. In case of a
 * default-constructed range, the function will attempt to determine suitable parameters
 * for the range settings.
 *
 * @param nBins The number of bins of the histogram
 * @param range The minimum and maximum boundaries of the histogram
 */
template<>
boost::shared_ptr<GDataCollector1T<double> >
GDataCollector2T<double, double>::projectY(std::size_t nBinsY, boost::tuple<double, double> rangeY) const {
	boost::tuple<double, double> myRangeY;
	if(rangeY == boost::tuple<double, double>()) {
		// Find out about the minimum and maximum values in the data_ array
		boost::tuple<double,double,double,double> extremes = Gem::Common::getMinMax(data_);
		myRangeY = boost::tuple<double,double>(boost::get<2>(extremes), boost::get<3>(extremes));
	} else {
		myRangeY = rangeY;
	}

	// Construct the result object
	boost::shared_ptr<GHistogram1D> result(new GHistogram1D(nBinsY, myRangeY));
	result->setXAxisLabel(this->yAxisLabel());
	result->setYAxisLabel("Number of entries");
	result->setPlotLabel(this->plotLabel() + " / y-projection");

	// Add data to the object
	for(std::size_t i=0; i<data_.size(); i++) {
		(*result) & boost::get<1>(data_.at(i));
	}

	// Return the data
	return result;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Specialization of projectX for <x_type, y_type, z_type> = <double, double, double>, that will return a
 * GHistogram1D object, wrapped into a boost::shared_ptr<GHistogram1D>. In case of a
 * default-constructed range, the function will attempt to determine suitable parameters
 * for the range settings.
 *
 * @param nBins The number of bins of the histogram
 * @param range The minimum and maximum boundaries of the histogram
 */
template<>
boost::shared_ptr<GDataCollector1T<double> >
GDataCollector3T<double, double,double>::projectX(std::size_t nBinsX, boost::tuple<double, double> rangeX) const {
   boost::tuple<double, double> myRangeX;
   if(rangeX == boost::tuple<double, double>()) {
      // Find out about the minimum and maximum values in the data_ array
      boost::tuple<double,double,double,double,double,double> extremes = Gem::Common::getMinMax(this->data_);
      myRangeX = boost::tuple<double,double>(boost::get<0>(extremes), boost::get<1>(extremes));
   } else {
      myRangeX = rangeX;
   }

   // Construct the result object
   boost::shared_ptr<GHistogram1D> result(new GHistogram1D(nBinsX, myRangeX));
   result->setXAxisLabel(this->xAxisLabel());
   result->setYAxisLabel("Number of entries");
   result->setPlotLabel(this->plotLabel() + " / x-projection");

   // Add data to the object
   for(std::size_t i=0; i<data_.size(); i++) {
      (*result) & boost::get<0>(data_.at(i));
   }

   // Return the data
   return result;
}

/******************************************************************************/
/**
 * Specialization of projectY for <x_type, y_type, z_type> = <double, double, double>, that will return a
 * GHistogram1D object, wrapped into a boost::shared_ptr<GHistogram1D>. In case of a
 * default-constructed range, the function will attempt to determine suitable parameters
 * for the range settings.
 *
 * @param nBins The number of bins of the histogram
 * @param range The minimum and maximum boundaries of the histogram
 */
template<>
boost::shared_ptr<GDataCollector1T<double> >
GDataCollector3T<double, double, double>::projectY(std::size_t nBinsY, boost::tuple<double, double> rangeY) const {
   boost::tuple<double, double> myRangeY;
   if(rangeY == boost::tuple<double, double>()) {
      // Find out about the minimum and maximum values in the data_ array
      boost::tuple<double,double,double,double,double,double> extremes = Gem::Common::getMinMax(data_);
      myRangeY = boost::tuple<double,double>(boost::get<2>(extremes), boost::get<3>(extremes));
   } else {
      myRangeY = rangeY;
   }

   // Construct the result object
   boost::shared_ptr<GHistogram1D> result(new GHistogram1D(nBinsY, myRangeY));
   result->setXAxisLabel(this->yAxisLabel());
   result->setYAxisLabel("Number of entries");
   result->setPlotLabel(this->plotLabel() + " / y-projection");

   // Add data to the object
   for(std::size_t i=0; i<data_.size(); i++) {
      (*result) & boost::get<1>(data_.at(i));
   }

   // Return the data
   return result;
}

/******************************************************************************/
/**
 * Specialization of projectZ for <x_type, y_type, z_type> = <double, double, double>, that will return a
 * GHistogram1D object, wrapped into a boost::shared_ptr<GHistogram1D>. In case of a
 * default-constructed range, the function will attempt to determine suitable parameters
 * for the range settings.
 *
 * @param nBins The number of bins of the histogram
 * @param range The minimum and maximum boundaries of the histogram
 */
template<>
boost::shared_ptr<GDataCollector1T<double> >
GDataCollector3T<double, double, double>::projectZ(std::size_t nBinsZ, boost::tuple<double, double> rangeZ) const {
   boost::tuple<double, double> myRangeZ;
   if(rangeZ == boost::tuple<double, double>()) {
      // Find out about the minimum and maximum values in the data_ array
      boost::tuple<double,double,double,double,double,double> extremes = Gem::Common::getMinMax(data_);
      myRangeZ = boost::tuple<double,double>(boost::get<4>(extremes), boost::get<5>(extremes));
   } else {
      myRangeZ = rangeZ;
   }

   // Construct the result object
   boost::shared_ptr<GHistogram1D> result(new GHistogram1D(nBinsZ, myRangeZ));
   result->setXAxisLabel(this->zAxisLabel());
   result->setYAxisLabel("Number of entries");
   result->setPlotLabel(this->plotLabel() + " / z-projection");

   // Add data to the object
   for(std::size_t i=0; i<data_.size(); i++) {
      (*result) & boost::get<2>(data_.at(i));
   }

   // Return the data
   return result;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor
 */
GGraph2D::GGraph2D()
	: pM_(DEFPLOTMODE)
	, drawArrows_(false)
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 */
GGraph2D::GGraph2D(const GGraph2D& cp)
	: GDataCollector2T<double,double>(cp)
	, pM_(cp.pM_)
	, drawArrows_(cp.drawArrows_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GGraph2D::~GGraph2D()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 */
const GGraph2D & GGraph2D::operator=(const GGraph2D& cp) {
	// Copy our parent class'es data
	GDataCollector2T<double,double>::operator=(cp);

	// and then our own
	pM_ = cp.pM_;
	drawArrows_ = cp.drawArrows_;

	return *this;
}

/******************************************************************************/
/**
 * Adds arrows to the plots between consecutive points. Note that setting this
 * value to true will force "SCATTER" mode
 *
 * @param dA The desired value of the drawArrows_ variable
 */
void GGraph2D::setDrawArrows(bool dA) {
	drawArrows_ = dA;
}

/******************************************************************************/
/**
 * Retrieves the value of the drawArrows_ variable
 *
 * @return The value of the drawArrows_ variable
 */
bool GGraph2D::getDrawArrows() const {
	return drawArrows_;
}

/******************************************************************************/
/**
 * Determines whether a scatter plot or a curve is created
 *
 * @param pM The desired plot mode
 */
void GGraph2D::setPlotMode(graphPlotMode pM) {
	pM_ = pM;
}

/******************************************************************************/
/**
 * Allows to retrieve the current plotting mode
 *
 * @return The current plot mode
 */
graphPlotMode GGraph2D::getPlotMode() const {
	return pM_;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 */
std::string GGraph2D::headerData() const {
	std::ostringstream header_data;

	// Set up suitable arrays for the header
	std::string arrayBaseName =
			"array_" + boost::lexical_cast<std::string>(id());

	std::string xArrayName = "x_" + arrayBaseName;
	std::string yArrayName = "y_" + arrayBaseName;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	}

	header_data
		<< "  double " << xArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << (comment!=""?comment:"") << std::endl
		<< "  double " << yArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << std::endl
		<< std::endl;

	return header_data.str();
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 */
std::string GGraph2D::bodyData() const {
	std::ostringstream body_data;

	// Set up suitable arrays for the header
	std::string arrayBaseName =
			"array_" + boost::lexical_cast<std::string>(id());

	std::string xArrayName = "x_" + arrayBaseName;
	std::string yArrayName = "y_" + arrayBaseName;

	std::string comment;
	if(dsMarker_ != "") {
		body_data << "// " + dsMarker_ << std::endl;
	}

	// Fill data from the tuples into the arrays
	std::vector<boost::tuple<double, double> >::const_iterator it;
	std::size_t posCounter = 0;

	for(it=data_.begin(); it!=data_.end(); ++it) {
		body_data
			<< "  " << xArrayName << "[" << posCounter << "] = " << boost::get<0>(*it) << ";" << "\t" << yArrayName << "[" << posCounter << "] = " << boost::get<1>(*it) << ";" << std::endl;

		posCounter++;
	}

	body_data << std::endl;

	return body_data.str();
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 */
std::string GGraph2D::footerData() const {
	std::ostringstream footer_data;

	// Set up suitable arrays for the header
	std::string arrayBaseName =
			"array_" + boost::lexical_cast<std::string>(id());

	std::string xArrayName = "x_" + arrayBaseName;
	std::string yArrayName = "y_" + arrayBaseName;

	std::string graphName = std::string("graph_") + boost::lexical_cast<std::string>(id());

	std::string comment;
	if(dsMarker_ != "") {
		footer_data << "// " + dsMarker_ << std::endl;
	}

	// Check whether custom drawing arguments have been set or whether one
	// of our generic choices has been selected
	std::string dA = "";
	if(drawingArguments() != "") {
		dA = drawingArguments();
	} else {
		if(pM_ == SCATTER || true==drawArrows_) {
			dA = "AP";
		} else {
			dA = "APL";
		}
	}

	// Fill the data in our tuple-vector into a ROOT TGraph object
	footer_data
		<< "  TGraph *" << graphName << " = new TGraph(" << data_.size() <<", " << xArrayName << ", " << yArrayName << ");" << std::endl
		<< "  " << graphName << "->GetXaxis()->SetTitle(\"" << xAxisLabel() << "\");" << std::endl
		<< "  " << graphName << "->GetYaxis()->SetTitle(\"" << yAxisLabel() << "\");" << std::endl;

	if(plot_label_ != "") {
		footer_data
			<< "  " << graphName << "->SetTitle(\"" << plot_label_ << "\");" << std::endl;
	}  else {
		footer_data
			<< "  " << graphName << "->SetTitle(\" \");" << std::endl;
	}

	footer_data
		<< "  " << graphName << "->Draw(\""<< dA << "\");" << std::endl
		<< std::endl;

	if(drawArrows_ && data_.size() >= 2) {
		std::vector<boost::tuple<double, double> >::const_iterator it;
		std::size_t posCounter = 0;

		double x1 = boost::get<0>(*data_.begin());
		double y1 = boost::get<1>(*data_.begin());
		double x2,y2;

		for(it=data_.begin()+1; it!=data_.end(); ++it) {
			x2 = boost::get<0>(*it);
			y2 = boost::get<1>(*it);

			footer_data
				<< "  TArrow * ta_" << graphName << "_" << posCounter << " = new TArrow(" << x1 << ", " << y1 << "," << x2 << ", " << y2 << ", " << 0.05 << ", \"|>\");" << std::endl
				<< "  ta_" << graphName << "_" << posCounter << "->SetArrowSize(0.01);" << std::endl
				<< "  ta_" << graphName << "_" << posCounter << "->Draw();" << std::endl;

			x1 = x2;
			y1 = y2;

			posCounter++;
		}
		footer_data
			<< std::endl;
	}

	return footer_data.str();
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor
 */
GGraph2ED::GGraph2ED()
	: pM_(DEFPLOTMODE)
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 */
GGraph2ED::GGraph2ED(const GGraph2ED& cp)
	: GDataCollector2ET<double,double>(cp)
	, pM_(cp.pM_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GGraph2ED::~GGraph2ED()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 */
const GGraph2ED & GGraph2ED::operator=(const GGraph2ED& cp) {
	// Copy our parent class'es data
	GDataCollector2ET<double,double>::operator=(cp);

	// and then our own
	pM_ = cp.pM_;

	return *this;
}

/******************************************************************************/
/**
 * Determines whether a scatter plot or a curve is created
 *
 * @param pM The desired plot mode
 */
void GGraph2ED::setPlotMode(graphPlotMode pM) {
	pM_ = pM;
}

/******************************************************************************/
/**
 * Allows to retrieve the current plotting mode
 *
 * @return The current plot mode
 */
graphPlotMode GGraph2ED::getPlotMode() const {
	return pM_;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 */
std::string GGraph2ED::headerData() const {
	std::ostringstream header_data;

	// Set up suitable arrays for the header
	std::string arrayBaseName =
			"array_" + boost::lexical_cast<std::string>(id());

	std::string  xArrayName = "x_" + arrayBaseName;
	std::string exArrayName = "ex_" + arrayBaseName;
	std::string  yArrayName = "y_" + arrayBaseName;
	std::string eyArrayName = "ey_" + arrayBaseName;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	}

	header_data
		<< "  double " <<  xArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << (comment!=""?comment:"") << std::endl
		<< "  double " << exArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << std::endl
		<< "  double " <<  yArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << std::endl
		<< "  double " << eyArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << std::endl
		<< std::endl;

	return header_data.str();
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 */
std::string GGraph2ED::bodyData() const {
	std::ostringstream body_data;

	// Set up suitable arrays for the header
	std::string arrayBaseName =
			"array_" + boost::lexical_cast<std::string>(id());

	std::string  xArrayName = "x_" + arrayBaseName;
	std::string exArrayName = "ex_" + arrayBaseName;
	std::string  yArrayName = "y_" + arrayBaseName;
	std::string eyArrayName = "ey_" + arrayBaseName;

	std::string comment;
	if(dsMarker_ != "") {
		body_data << "// " + dsMarker_ << std::endl;
	}

	// Fill data from the tuples into the arrays
	std::vector<boost::tuple<double, double, double, double> >::const_iterator it;
	std::size_t posCounter = 0;

	for(it=data_.begin(); it!=data_.end(); ++it) {
		body_data
			<< "  " <<  xArrayName << "[" << posCounter << "] = " << boost::get<0>(*it) << ";" << std::endl
			<< "  " << exArrayName << "[" << posCounter << "] = " << boost::get<1>(*it) << ";" << std::endl
			<< "  " <<  yArrayName << "[" << posCounter << "] = " << boost::get<2>(*it) << ";" << std::endl
			<< "  " << eyArrayName << "[" << posCounter << "] = " << boost::get<3>(*it) << ";" << std::endl;

		posCounter++;
	}

	body_data << std::endl;

	return body_data.str();
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 */
std::string GGraph2ED::footerData() const {
	std::ostringstream footer_data;

	// Set up suitable arrays for the header
	std::string arrayBaseName =
			"array_" + boost::lexical_cast<std::string>(id());

	std::string  xArrayName = "x_" + arrayBaseName;
	std::string exArrayName = "ex_" + arrayBaseName;
	std::string  yArrayName = "y_" + arrayBaseName;
	std::string eyArrayName = "ey_" + arrayBaseName;

	std::string graphName = std::string("graph_") + boost::lexical_cast<std::string>(id());

	std::string comment;
	if(dsMarker_ != "") {
		footer_data << "// " + dsMarker_ << std::endl;
	}

	// Check whether custom drawing arguments have been set or whether one
	// of our generic choices has been selected
	std::string dA = "";
	if(drawingArguments() != "") {
		dA = drawingArguments();
	} else {
		if(pM_ == SCATTER) {
			dA = "AP";
		} else {
			dA = "APL";
		}
	}

	// Fill the data in our tuple-vector into a ROOT TGraphErrors object
	footer_data
		<< "  TGraphErrors *" << graphName << " = new TGraphErrors(" << data_.size() <<", " << xArrayName << ", " << yArrayName << ", " << exArrayName << " ," << eyArrayName << ");" << std::endl
		<< "  " << graphName << "->GetXaxis()->SetTitle(\"" << xAxisLabel() << "\");" << std::endl
		<< "  " << graphName << "->GetYaxis()->SetTitle(\"" << yAxisLabel() << "\");" << std::endl;

	if(plot_label_ != "") {
		footer_data
			<< "  " << graphName << "->SetTitle(\"" << plot_label_ << "\");" << std::endl;
	}  else {
		footer_data
			<< "  " << graphName << "->SetTitle(\" \");" << std::endl;
	}

	footer_data
		<< "  " << graphName << "->Draw(\""<< dA << "\");" << std::endl
		<< std::endl;

	return footer_data.str();
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor
 */
GGraph3D::GGraph3D()
   : drawLines_(false)
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 */
GGraph3D::GGraph3D(const GGraph3D& cp)
   : GDataCollector3T<double,double,double>(cp)
   , drawLines_(cp.drawLines_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GGraph3D::~GGraph3D()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 */
const GGraph3D & GGraph3D::operator=(const GGraph3D& cp) {
   // Copy our parent class'es data
   GDataCollector3T<double,double,double>::operator=(cp);

   // and then our own
   drawLines_ = cp.drawLines_;

   return *this;
}

/******************************************************************************/
/**
 * Adds lines to the plots between consecutive points.
 *
 * @param dL The desired value of the drawLines_ variable
 */
void GGraph3D::setDrawLines(bool dL) {
   drawLines_ = dL;
}

/******************************************************************************/
/**
 * Retrieves the value of the drawLines_ variable
 *
 * @return The value of the drawLines_ variable
 */
bool GGraph3D::getDrawLines() const {
   return drawLines_;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 */
std::string GGraph3D::headerData() const {
   std::ostringstream header_data;

   // Set up suitable arrays for the header
   std::string arrayBaseName =
         "array_" + boost::lexical_cast<std::string>(id());

   std::string xArrayName = "x_" + arrayBaseName;
   std::string yArrayName = "y_" + arrayBaseName;
   std::string zArrayName = "z_" + arrayBaseName;

   std::string comment;
   if(dsMarker_ != "") {
      comment = "// " + dsMarker_;
   }

   header_data
      << "  double " << xArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << (comment!=""?comment:"") << std::endl
      << "  double " << yArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << std::endl
      << "  double " << zArrayName << "[" << boost::lexical_cast<std::string>(data_.size()) << "];" << std::endl
      << std::endl;

   return header_data.str();
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 */
std::string GGraph3D::bodyData() const {
   std::ostringstream body_data;

   // Set up suitable arrays for the header
   std::string arrayBaseName =
         "array_" + boost::lexical_cast<std::string>(id());

   std::string xArrayName = "x_" + arrayBaseName;
   std::string yArrayName = "y_" + arrayBaseName;
   std::string zArrayName = "z_" + arrayBaseName;

   std::string comment;
   if(dsMarker_ != "") {
      body_data << "// " + dsMarker_ << std::endl;
   }

   // Fill data from the tuples into the arrays
   std::vector<boost::tuple<double, double, double> >::const_iterator it;
   std::size_t posCounter = 0;

   for(it=data_.begin(); it!=data_.end(); ++it) {
      body_data
         << "  " << xArrayName << "[" << posCounter << "] = " << boost::get<0>(*it) << ";" << "\t"
                 << yArrayName << "[" << posCounter << "] = " << boost::get<1>(*it) << ";" << "\t"
                 << zArrayName << "[" << posCounter << "] = " << boost::get<2>(*it) << ";" << std::endl;

      posCounter++;
   }

   body_data << std::endl;

   return body_data.str();
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 */
std::string GGraph3D::footerData() const {
   std::ostringstream footer_data;

   // Set up suitable arrays for the header
   std::string arrayBaseName =
         "array_" + boost::lexical_cast<std::string>(id());

   std::string xArrayName = "x_" + arrayBaseName;
   std::string yArrayName = "y_" + arrayBaseName;
   std::string zArrayName = "z_" + arrayBaseName;

   std::string graphName = std::string("graph_") + boost::lexical_cast<std::string>(id());

   std::string comment;
   if(dsMarker_ != "") {
      footer_data << "// " + dsMarker_ << std::endl;
   }

   // Check whether custom drawing arguments have been set or whether one
   // of our generic choices has been selected
   std::string dA = drawingArguments();
   if(dA != "") {
      dA = drawingArguments();
   } else {
      dA = "AP";
   }

   // Fill the data in our tuple-vector into a ROOT TGraph object
   footer_data
      << "  TGraph2D *" << graphName << " = new TGraph2D(" << data_.size() <<", " << xArrayName << ", " << yArrayName << ", " << zArrayName << ");" << std::endl
      << "  " << graphName << "->GetXaxis()->SetTitle(\"" << xAxisLabel() << "\");" << std::endl
      << "  " << graphName << "->GetYaxis()->SetTitle(\"" << yAxisLabel() << "\");" << std::endl
      << "  " << graphName << "->GetZaxis()->SetTitle(\"" << zAxisLabel() << "\");" << std::endl;

   if(plot_label_ != "") {
      footer_data
         << "  " << graphName << "->SetTitle(\"" << plot_label_ << "\");" << std::endl;
   }  else {
      footer_data
         << "  " << graphName << "->SetTitle(\" \");" << std::endl;
   }

   footer_data
      << "  " << graphName << "->Draw(\""<< dA << "\");" << std::endl
      << std::endl;

   if(drawLines_ && data_.size() >= 2) {
      std::vector<boost::tuple<double, double, double> >::const_iterator it;
      std::size_t posCounter = 0;

      double x,y,z;

      footer_data
      << "  TPolyLine3D *lines_" << graphName << " = new TPolyLine3D(" << data_.size() << ");" << std::endl
      << std::endl;

      for(it=data_.begin()+1; it!=data_.end(); ++it) {
         x = boost::get<0>(*it);
         y = boost::get<1>(*it);
         z = boost::get<2>(*it);

         footer_data
         << "  lines_" << graphName << "->SetPoint(" << posCounter << ", " << x << ", " << y << ", " << z << ");";

         posCounter++;
      }
      footer_data
      << std::endl
      << "  lines_" << graphName << "->SetLineWidth(3);" << std::endl
      << "  lines_" << graphName << "->Draw();" << std::endl
      << std::endl;
   }

   return footer_data.str();
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 */
GHistogram1D::GHistogram1D(
	const std::size_t& nBinsX
	, const double& minX
	, const double& maxX
)
	: nBinsX_(nBinsX)
	, minX_(minX)
	, maxX_(maxX)
{ /* nothing */ }

/******************************************************************************/
/**
 * Initialization with a range in the form of a tuple
 */
GHistogram1D::GHistogram1D(
	const std::size_t& nBinsX
	, const boost::tuple<double,double>& rangeX
)
	: nBinsX_(nBinsX)
	, minX_(boost::get<0>(rangeX))
	, maxX_(boost::get<1>(rangeX))
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 *
 * @param cp a copy of another GHistogram1D object
 */
GHistogram1D::GHistogram1D(const GHistogram1D& cp)
	: GDataCollector1T<double>(cp)
	, nBinsX_(cp.nBinsX_)
	, minX_(cp. minX_)
	, maxX_(cp.maxX_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GHistogram1D::~GHistogram1D()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 *
 * @param cp A copy of another GHistogram1D object
 */
const GHistogram1D &GHistogram1D::operator=(const GHistogram1D& cp) {
	// Copy our parent class'es data ...
	GDataCollector1T<double>::operator=(cp);

	// ... and then our local data
	nBinsX_ = cp.nBinsX_;
	minX_ = cp. minX_;
	maxX_ = cp.maxX_;

	return *this;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 */
std::string GHistogram1D::headerData() const {
	std::ostringstream header_data;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	}

	std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

	header_data
		<< "  TH1D *" << histName << " = new TH1D(\"" << histName << "\", \"" << histName << "\"," << nBinsX_ << ", " << minX_ << ", " << maxX_ << ");" << (comment!=""?comment:"") << std::endl
		<< std::endl;

	return header_data.str();
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 */
std::string GHistogram1D::bodyData() const {
	std::ostringstream body_data;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	} else {
		comment = "";
	}

	std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

	std::vector<double>::const_iterator it;
	std::size_t posCounter = 0;
	for(it=data_.begin(); it!=data_.end(); ++it) {
		body_data
			<< "  " << histName << "->Fill(" << *it << ");" << (posCounter==0?comment:("")) << std::endl;
		posCounter++;
	}

	body_data << std::endl;

	return body_data.str();
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 */
std::string GHistogram1D::footerData() const {
	std::ostringstream footer_data;

	std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

	if(plot_label_ != "") {
		footer_data
			<< "  " << histName << "->SetTitle(\"" << plot_label_ << "\");" << std::endl;
	}  else {
		footer_data
			<< "  " << histName << "->SetTitle(\" \");" << std::endl;
	}

	std::string comment;
	if(dsMarker_ != "") {
		footer_data << "// " + dsMarker_ << std::endl;
	}

	// Check whether custom drawing arguments have been set
	std::string dA = "";
	if(drawingArguments() != "") {
		dA = drawingArguments();
	}

	footer_data
	    << "  " << histName << "->GetXaxis()->SetTitle(\"" << xAxisLabel() << "\");" << std::endl
	    << "  " << histName << "->GetYaxis()->SetTitle(\"" << yAxisLabel() << "\");" << std::endl
		<< "  " << histName << "->Draw(\""<< dA << "\");" << std::endl
		<< std::endl;

	return footer_data.str();
}

/******************************************************************************/
/**
 * Retrieve the number of bins in x-direction
 *
 * @return The number of bins in x-direction
 */
std::size_t GHistogram1D::getNBinsX() const {
	return nBinsX_;
}

/******************************************************************************/
/**
 * Retrieve the lower boundary of the plot
 *
 * @return The lower boundary of the plot
 */
double GHistogram1D::getMinX() const {
	return minX_;
}

/******************************************************************************/
/**
 * Retrieve the upper boundary of the plot
 *
 * @return The upper boundary of the plot
 */
double GHistogram1D::getMaxX() const {
	return maxX_;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 */
GHistogram1I::GHistogram1I(
   const std::size_t& nBinsX
   , const double& minX
   , const double& maxX
)
   : GDataCollector1T<boost::int32_t>()
   , nBinsX_(nBinsX)
   , minX_(minX)
   , maxX_(maxX)
{ /* nothing */ }

/******************************************************************************/
/**
 * Initialization with a range in the form of a tuple
 */
GHistogram1I::GHistogram1I(
   const std::size_t& nBinsX
   , const boost::tuple<double,double>& rangeX
)
   : GDataCollector1T<boost::int32_t>()
   , nBinsX_(nBinsX)
   , minX_(boost::get<0>(rangeX))
   , maxX_(boost::get<1>(rangeX))
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 *
 * @param cp a copy of another GHistogram1I object
 */
GHistogram1I::GHistogram1I(const GHistogram1I& cp)
   : GDataCollector1T<boost::int32_t>(cp)
   , nBinsX_(cp.nBinsX_)
   , minX_(cp. minX_)
   , maxX_(cp.maxX_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GHistogram1I::~GHistogram1I()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 *
 * @param cp A copy of another GHistogram1I object
 */
const GHistogram1I &GHistogram1I::operator=(const GHistogram1I& cp) {
   // Copy our parent class'es data ...
   GDataCollector1T<boost::int32_t>::operator=(cp);

   // ... and then our local data
   nBinsX_ = cp.nBinsX_;
   minX_ = cp. minX_;
   maxX_ = cp.maxX_;

   return *this;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 */
std::string GHistogram1I::headerData() const {
   std::ostringstream header_data;

   std::string comment;
   if(dsMarker_ != "") {
      comment = "// " + dsMarker_;
   }

   std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

   header_data
      << "  TH1I *" << histName << " = new TH1I(\"" << histName << "\", \"" << histName << "\"," << nBinsX_ << ", " << minX_ << ", " << maxX_ << ");" << (comment!=""?comment:"") << std::endl
      << std::endl;

   return header_data.str();
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 */
std::string GHistogram1I::bodyData() const {
   std::ostringstream body_data;

   std::string comment;
   if(dsMarker_ != "") {
      comment = "// " + dsMarker_;
   } else {
      comment = "";
   }

   std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

   std::vector<boost::int32_t>::const_iterator it;
   std::size_t posCounter = 0;
   for(it=data_.begin(); it!=data_.end(); ++it) {
      body_data
         << "  " << histName << "->Fill(" << *it << ");" << (posCounter==0?comment:("")) << std::endl;
      posCounter++;
   }

   body_data << std::endl;

   return body_data.str();
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 */
std::string GHistogram1I::footerData() const {
   std::ostringstream footer_data;

   std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

   if(plot_label_ != "") {
      footer_data
         << "  " << histName << "->SetTitle(\"" << plot_label_ << "\");" << std::endl;
   }  else {
      footer_data
         << "  " << histName << "->SetTitle(\" \");" << std::endl;
   }

   std::string comment;
   if(dsMarker_ != "") {
      footer_data << "// " + dsMarker_ << std::endl;
   }

   // Check whether custom drawing arguments have been set
   std::string dA = "";
   if(drawingArguments() != "") {
      dA = drawingArguments();
   }

   footer_data
       << "  " << histName << "->GetXaxis()->SetTitle(\"" << xAxisLabel() << "\");" << std::endl
       << "  " << histName << "->GetYaxis()->SetTitle(\"" << yAxisLabel() << "\");" << std::endl
      << "  " << histName << "->Draw(\""<< dA << "\");" << std::endl
      << std::endl;

   return footer_data.str();
}

/******************************************************************************/
/**
 * Retrieve the number of bins in x-direction
 *
 * @return The number of bins in x-direction
 */
std::size_t GHistogram1I::getNBinsX() const {
   return nBinsX_;
}

/******************************************************************************/
/**
 * Retrieve the lower boundary of the plot
 *
 * @return The lower boundary of the plot
 */
double GHistogram1I::getMinX() const {
   return minX_;
}

/******************************************************************************/
/**
 * Retrieve the upper boundary of the plot
 *
 * @return The upper boundary of the plot
 */
double GHistogram1I::getMaxX() const {
   return maxX_;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 */
GHistogram2D::GHistogram2D(
	const std::size_t& nBinsX
	, const std::size_t& nBinsY
	, const double& minX
	, const double& maxX
	, const double& minY
	, const double& maxY
)
	: nBinsX_(nBinsX)
	, nBinsY_(nBinsY)
	, minX_(minX)
	, maxX_(maxX)
	, minY_(minY)
	, maxY_(maxY)
	, dropt_(TDEMPTY)
{ /* nothing */ }

/******************************************************************************/
/**
 * Initialization with ranges
 */
GHistogram2D::GHistogram2D(
	const std::size_t& nBinsX
	, const std::size_t& nBinsY
	, const boost::tuple<double,double>& rangeX
	, const boost::tuple<double,double>& rangeY
)
	: nBinsX_(nBinsX)
	, nBinsY_(nBinsY)
	, minX_(boost::get<0>(rangeX))
	, maxX_(boost::get<1>(rangeX))
	, minY_(boost::get<0>(rangeY))
	, maxY_(boost::get<1>(rangeY))
	, dropt_(TDEMPTY)
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 *
 * @param cp a copy of another GHistogram2D object
 */
GHistogram2D::GHistogram2D(const GHistogram2D& cp)
	: GDataCollector2T<double, double>(cp)
	, nBinsX_(cp.nBinsX_)
	, nBinsY_(cp.nBinsY_)
	, minX_(cp. minX_)
	, maxX_(cp.maxX_)
	, minY_(cp. minY_)
	, maxY_(cp.maxY_)
	, dropt_(cp.dropt_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GHistogram2D::~GHistogram2D()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 *
 * @param cp A copy of another GHistogram2D object
 */
const GHistogram2D &GHistogram2D::operator=(const GHistogram2D& cp) {
	// Copy our parent class'es data ...
	GDataCollector2T<double,double>::operator=(cp);

	// ... and then our local data
	nBinsX_ = cp.nBinsX_;
	nBinsY_ = cp.nBinsY_;
	minX_ = cp. minX_;
	maxX_ = cp.maxX_;
	minY_ = cp. minY_;
	maxY_ = cp.maxY_;
	dropt_ = cp.dropt_;

	return *this;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 */
std::string GHistogram2D::headerData() const {
	std::ostringstream header_data;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	}

	std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

	header_data
		<< "  TH2D *"
		<< histName
		<< " = new TH2D(\""
		<< histName
		<< "\", \""
		<< histName
		<< "\","
		<< nBinsX_
		<< ", "
		<< minX_
		<< ", "
		<< maxX_
		<< ","
		<< nBinsY_
		<< ", "
		<< minY_
		<< ", "
		<< maxY_
		<< ");"
		<< (comment!=""?comment:"")
		<< std::endl
		<< std::endl;

	return header_data.str();
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 */
std::string GHistogram2D::bodyData() const {
	std::ostringstream body_data;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	} else {
		comment = "";
	}

	std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

	std::vector<boost::tuple<double,double> >::const_iterator it;
	std::size_t posCounter = 0;
	for(it=data_.begin(); it!=data_.end(); ++it) {
		body_data
			<< "  " << histName << "->Fill(" << boost::get<0>(*it) << ", " << boost::get<1>(*it) << ");" << (posCounter==0?comment:("")) << std::endl;
		posCounter++;
	}

	body_data << std::endl;

	return body_data.str();
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 */
std::string GHistogram2D::footerData() const {
	std::ostringstream footer_data;

	std::string histName = "hist_" + boost::lexical_cast<std::string>(id());

	if(plot_label_ != "") {
		footer_data
			<< "  " << histName << "->SetTitle(\"" << plot_label_ << "\");" << std::endl;
	}  else {
		footer_data
			<< "  " << histName << "->SetTitle(\" \");" << std::endl;
	}

	std::string comment;
	if(dsMarker_ != "") {
		footer_data << "// " + dsMarker_ << std::endl;
	}

	// Check whether custom drawing arguments have been set
	std::string dA = "";
	if(drawingArguments() != "") {
		dA = drawingArguments();
	} else {
		switch(dropt_) {
		case TDEMPTY:
			dA = "";
			break;

		case SURFONE:
			dA = "SURF1";
			break;

		case SURFTWOZ:
			dA = "SURF2Z";
			break;

		case SURFTHREE:
			dA = "SURF3";
			break;

		case SURFFOUR:
			dA = "SURF4";
			break;

		case CONTZ:
			dA = "CONTZ";
			break;

		case CONTONE:
			dA = "CONT1";
			break;

		case CONTTWO:
			dA = "CONT2";
			break;

		case CONTTHREE:
			dA = "CONT3";
			break;

		case TEXT:
			dA = "TEXT";
			break;

		case SCAT:
			dA = "SCAT";
			break;

		case BOX:
			dA = "BOX";
			break;

		case ARR:
			dA = "ARR";
			break;

		case COLZ:
			dA = "COLZ";
			break;

		case LEGO:
			dA = "LEGO";
			break;

		case LEGOONE:
			dA = "LEGO1";
			break;

		case SURFONEPOL:
			dA = "SURF1POL";
			break;

		case SURFONECYL:
			dA = "SURF1CYL";
			break;

		default:
		{
		   glogger
		   << "In GHistogram2D::footerData(): Error!" << std::endl
         << "Encountered unknown drawing option " << dropt_ << std::endl
         << GEXCEPTION;
		}
		break;

		}
	}

	footer_data
	<< "  " << histName << "->GetXaxis()->SetTitle(\"" << xAxisLabel() << "\");" << std::endl
	<< "  " << histName << "->GetYaxis()->SetTitle(\"" << yAxisLabel() << "\");" << std::endl
	<< "  " << histName << "->Draw(\""<< dA << "\");" << std::endl
	<< std::endl;

	return footer_data.str();
}

/******************************************************************************/
/**
 * Retrieve the number of bins in x-direction
 *
 * @return The number of bins in x-direction
 */
std::size_t GHistogram2D::getNBinsX() const {
	return nBinsX_;
}

/******************************************************************************/
/**
 * Retrieve the number of bins in y-direction
 *
 * @return The number of bins in y-direction
 */
std::size_t GHistogram2D::getNBinsY() const {
	return nBinsY_;
}

/******************************************************************************/
/**
 * Retrieve the lower boundary of the plot in x-direction
 *
 * @return The lower boundary of the plot in x-direction
 */
double GHistogram2D::getMinX() const {
	return minX_;
}

/******************************************************************************/
/**
 * Retrieve the upper boundary of the plot in x-direction
 *
 * @return The upper boundary of the plot in x-direction
 */
double GHistogram2D::getMaxX() const {
	return maxX_;
}

/******************************************************************************/
/**
 * Retrieve the lower boundary of the plot in y-direction
 *
 * @return The lower boundary of the plot in y-direction
 */
double GHistogram2D::getMinY() const {
	return minY_;
}

/******************************************************************************/
/**
 * Retrieve the upper boundary of the plot in y-direction
 *
 * @return The upper boundary of the plot in y-direction
 */
double GHistogram2D::getMaxY() const {
	return maxY_;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 *
 * @param fD A descriptor for the function to be plotted
 */
GFunctionPlotter1D::GFunctionPlotter1D(
		const std::string& fD
		, const boost::tuple<double,double>& xExtremes
)
	: functionDescription_(fD)
	, xExtremes_(xExtremes)
	, nSamplesX_(DEFNSAMPLES)
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 *
 * @param cp A reference to another GFunctionPlotter1D object
 */
GFunctionPlotter1D::GFunctionPlotter1D(const GFunctionPlotter1D& cp)
	: GBasePlotter(cp)
	, functionDescription_(cp.functionDescription_)
	, xExtremes_(cp.xExtremes_)
	, nSamplesX_(cp.nSamplesX_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GFunctionPlotter1D::~GFunctionPlotter1D()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 *
 * @param cp A reference to another GFunctionPlotter1D object
 */
const GFunctionPlotter1D & GFunctionPlotter1D::operator=(const GFunctionPlotter1D& cp) {
	// Copy our parent class'es data
	GBasePlotter::operator=(cp);

	// and then our local data
	functionDescription_ = cp.functionDescription_;
	xExtremes_ = cp.xExtremes_;
	nSamplesX_ = cp.nSamplesX_;

	return *this;
}

/******************************************************************************/
/**
 * Allows to set the number of sampling points of the function on the x-axis
 *
 * @param nSamplesX The number of sampling points of the function on the x-axis
 */
void GFunctionPlotter1D::setNSamplesX(std::size_t nSamplesX) {
	nSamplesX_ = nSamplesX;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 *
 * @return The code to be added to the plot header for this function
 */
std::string GFunctionPlotter1D::headerData() const {
	// Check the extreme values for consistency
	if(boost::get<0>(xExtremes_) >= boost::get<1>(xExtremes_)) {
	   glogger
	   << "In GFunctionPlotter1D::headerData(): Error!" << std::endl
      << "lower boundary >= upper boundary: " << boost::get<0>(xExtremes_) << " / " << boost::get<1>(xExtremes_) << std::endl
      << GEXCEPTION;
	}

	std::ostringstream result;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	}

	std::string functionName = "func_" + boost::lexical_cast<std::string>(id());
	result << "  TF1 *" << functionName << " = new TF1(\"" << functionName << "\", \"" << functionDescription_ << "\"," << boost::get<0>(xExtremes_) << ", " << boost::get<1>(xExtremes_) << ");" << (comment!=""?comment:"") << std::endl;

	return result.str();
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 *
 * @return The code to be added to the plot's data section for this function
 */
std::string GFunctionPlotter1D::bodyData() const {
	// No data needs to be added for a function plotter
	return std::string();
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 *
 * @return The draw command to be added to the plot's data for this function
 */
std::string GFunctionPlotter1D::footerData() const {
	std::ostringstream footer_data;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	}

	std::string functionName = "func_" + boost::lexical_cast<std::string>(id());
	footer_data
		<< "  " << functionName << "->GetXaxis()->SetTitle(\"" << xAxisLabel() << "\");" << std::endl
		<< "  " << functionName << "->GetYaxis()->SetTitle(\"" << yAxisLabel() << "\");" << std::endl
	    << "  " << functionName << "->SetNpx(" << nSamplesX_ << ");" << std::endl;

	if(plot_label_ != "") {
		footer_data
			<< "  " << functionName << "->SetTitle(\"" << plot_label_ << "\");" << std::endl;
	} else {
		footer_data
			<< "  " << functionName << "->SetTitle(\" \");" << std::endl;
	}

	std::string dA;
	if(drawingArguments() == "") {
		dA = "";
	} else {
		dA = "\"" + drawingArguments() + "\"";
	}

	footer_data
		<< "  " << functionName << "->Draw(" << dA << ");" << (comment!=""?comment:"") << std::endl
		<< std::endl;

	return footer_data.str();
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 *
 * @param fD A descriptor for the function to be plotted
 */
GFunctionPlotter2D::GFunctionPlotter2D(
		const std::string& fD
		, const boost::tuple<double,double>& xExtremes
		, const boost::tuple<double,double>& yExtremes
)
	: functionDescription_(fD)
	, xExtremes_(xExtremes)
	, yExtremes_(yExtremes)
	, nSamplesX_(DEFNSAMPLES)
	, nSamplesY_(DEFNSAMPLES)
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 *
 * @param cp A reference to another GFunctionPlotter2D object
 */
GFunctionPlotter2D::GFunctionPlotter2D(const GFunctionPlotter2D& cp)
	: GBasePlotter(cp)
	, functionDescription_(cp.functionDescription_)
	, xExtremes_(cp.xExtremes_)
	, yExtremes_(cp.yExtremes_)
	, nSamplesX_(cp.nSamplesX_)
	, nSamplesY_(cp.nSamplesY_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GFunctionPlotter2D::~GFunctionPlotter2D()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 *
 * @param cp A reference to another GFunctionPlotter2D object
 */
const GFunctionPlotter2D & GFunctionPlotter2D::operator=(const GFunctionPlotter2D& cp) {
	// Copy our parent class'es data
	GBasePlotter::operator=(cp);

	// and then our local data
	functionDescription_ = cp.functionDescription_;
	xExtremes_ = cp.xExtremes_;
	yExtremes_ = cp.yExtremes_;
	nSamplesX_ = cp.nSamplesX_;
	nSamplesY_ = cp.nSamplesY_;

	return *this;
}

/******************************************************************************/
/**
 * Allows to set the number of sampling points of the function on the x-axis
 *
 * @param nSamplesX The number of sampling points of the function on the x-axis
 */
void GFunctionPlotter2D::setNSamplesX(std::size_t nSamplesX) {
	nSamplesX_ = nSamplesX;
}

/******************************************************************************/
/**
 * Allows to set the number of sampling points of the function on the y-axis
 *
 * @param nSamplesY The number of sampling points of the function on the y-axis
 */
void GFunctionPlotter2D::setNSamplesY(std::size_t nSamplesY) {
	nSamplesY_ = nSamplesY;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 *
 * @return The code to be added to the plot header for this function
 */
std::string GFunctionPlotter2D::headerData() const {
	// Check the extreme values for consistency
	if(boost::get<0>(xExtremes_) >= boost::get<1>(xExtremes_)) {
	   glogger
	   << "In GFunctionPlotter2D::headerData(): Error!" << std::endl
      << "lower boundary(x) >= upper boundary(x): " << boost::get<0>(xExtremes_) << " / " << boost::get<1>(xExtremes_) << std::endl
      << GEXCEPTION;
	}

	if(boost::get<0>(yExtremes_) >= boost::get<1>(yExtremes_)) {
	   glogger
	   << "In GFunctionPlotter2D::headerData(): Error!" << std::endl
      << "lower boundary(y) >= upper boundary(y): " << boost::get<0>(yExtremes_) << " / " << boost::get<1>(yExtremes_) << std::endl
      << GEXCEPTION;
	}

	std::ostringstream result;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	}

	std::string functionName = "func_" + boost::lexical_cast<std::string>(id());
	result
		<< "  TF2 *"
		<< functionName
		<< " = new TF2(\""
		<< functionName << "\", \""
		<< functionDescription_
		<< "\","
		<< boost::get<0>(xExtremes_)
		<< ", "
		<< boost::get<1>(xExtremes_)
		<< ", "
		<< boost::get<0>(yExtremes_)
		<< ", "
		<< boost::get<1>(yExtremes_)
		<< ");"
		<< (comment!=""?comment:"")
		<< std::endl;

	return result.str();
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 *
 * @return The code to be added to the plot's data section for this function
 */
std::string GFunctionPlotter2D::bodyData() const {
	// No data needs to be added for a function plotter
	return std::string();
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 *
 * @return The draw command to be added to the plot's data for this function
 */
std::string GFunctionPlotter2D::footerData() const {
	std::ostringstream footer_data;

	std::string comment;
	if(dsMarker_ != "") {
		comment = "// " + dsMarker_;
	}

	std::string functionName = "func_" + boost::lexical_cast<std::string>(id());
	footer_data
		<< "  " << functionName << "->GetXaxis()->SetTitle(\"" << xAxisLabel() << "\");" << std::endl
		<< "  " << functionName << "->GetYaxis()->SetTitle(\"" << yAxisLabel() << "\");" << std::endl
		<< "  " << functionName << "->GetZaxis()->SetTitle(\"" << zAxisLabel() << "\");" << std::endl
	    << "  " << functionName << "->SetNpx(" << nSamplesX_ << ");" << std::endl
	    << "  " << functionName << "->SetNpy(" << nSamplesY_ << ");" << std::endl;

	if(plot_label_ != "") {
		footer_data
			<< "  " << functionName << "->SetTitle(\"" << plot_label_ << "\");" << std::endl;
	} else {
		footer_data
			<< "  " << functionName << "->SetTitle(\" \");" << std::endl;
	}

	std::string dA;
	if(drawingArguments() == "") {
		dA = "";
	} else {
		dA = "\"" + drawingArguments() + "\"";
	}

	footer_data
		<< "  " << functionName << "->Draw(" << dA << ");" << (comment!=""?comment:"") << std::endl
		<< std::endl;

	return footer_data.str();
}


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The default constructor
 */
GFreeFormPlotter::GFreeFormPlotter()
	: headerData_("")
	, bodyData_("")
	, footerData_("")
{ /* nothing */ }

/******************************************************************************/
/**
 * A copy constructor
 *
 * @param cp A copy of another GFreeFormPlotter object
 */
GFreeFormPlotter::GFreeFormPlotter(const GFreeFormPlotter& cp)
	: GBasePlotter(cp)
	, headerData_(cp.headerData_)
	, bodyData_(cp.bodyData_)
	, footerData_(cp.footerData_)
{ /* nothing */ }

/******************************************************************************/
/**
 * The destructor
 */
GFreeFormPlotter::~GFreeFormPlotter()
{ /* nothing */ }

/******************************************************************************/
/**
 * The assignment operator
 *
 * @param cp A copy of another GFreeFormPlotter object
 */
const GFreeFormPlotter& GFreeFormPlotter::operator=(const GFreeFormPlotter& cp)
{
	// Copy our parent class'es data ...
	GBasePlotter::operator=(cp);

	// ... and then our local data
	headerData_ = cp.headerData_;
	bodyData_ = cp.bodyData_;
	footerData_ = cp.footerData_;

	return *this;
}

/******************************************************************************/
/**
 * Retrieve specific header settings for this plot
 *
 * @return A string holding the header data
 */
std::string GFreeFormPlotter::headerData() const {
	return headerData_;
}

/******************************************************************************/
/**
 * Retrieves the actual data sets
 *
 * @return A string holding the body data
 */
std::string GFreeFormPlotter::bodyData() const {
	return bodyData_;
}

/******************************************************************************/
/**
 * Retrieves specific draw commands for this plot
 *
 * @return A string holding the footer data
 */
std::string GFreeFormPlotter::footerData() const {
	return footerData_;
}

/******************************************************************************/
/**
 *
 */
void GFreeFormPlotter::setHeaderData(const std::string& hD) {
	headerData_ = hD;
}

/******************************************************************************/
/**
 *
 */
void GFreeFormPlotter::setBodyData(const std::string& bD){
	bodyData_ = bD;
}

/******************************************************************************/
/**
 *
 */
void GFreeFormPlotter::setFooterData(const std::string& fD) {
	footerData_ = fD;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The standard constructor
 *
 * @param The label of the canvas
 * @param c_x_div The number of plots in x-direction
 * @param c_y_div The number of plots in y-direction
 */
GPlotDesigner::GPlotDesigner(
	const std::string& canvasLabel
	, const std::size_t& c_x_div
	, const std::size_t& c_y_div
)
	: c_x_div_(c_x_div)
	, c_y_div_(c_y_div)
	, c_x_dim_(DEFCXDIM)
	, c_y_dim_(DEFCYDIM)
	, canvasLabel_(canvasLabel)
{ /* nothing */ }

/******************************************************************************/
/**
 * Writes the plot to a file
 *
 * @param fileName The name of the file to which the data can be written
 */
void GPlotDesigner::writeToFile(const std::string& fileName) {
	std::ofstream result(fileName.c_str());
	result << plot();
	result.close();
}

/******************************************************************************/
/*
 * Emits the overall plot
 */
std::string GPlotDesigner::plot() const {
	std::ostringstream result;
	std::size_t maxPlots = c_x_div_*c_y_div_;

#ifdef DEBUG
	if(plotters_.size() > maxPlots) {
		std::cerr << "In GPlotDesigner::plot(): Warning!" << std::endl
				  << "Found more plots than pads." << std::endl
				  << "Some of the plots will be ignored" << std::endl;
	}
#endif /* DEBUG*/

	result
		<< "{" << std::endl
		<<    staticHeader()
		<< std::endl;

	// Plot all body sections up to the maximum allowed number
	result << "  //===================  Header Section ====================" << std::endl
		   << std::endl;

	// Plot all headers up to the maximum allowed number
	std::size_t nPlots = 0;
	std::vector<boost::shared_ptr<GBasePlotter> >::const_iterator it;
	for(it=plotters_.begin(); it!=plotters_.end(); ++it) {
		if(nPlots++ < maxPlots) {
			result << (*it)->headerData()
				   << std::endl;
		}
	}

	// Plot all body sections up to the maximum allowed number
	result << "  //===================  Data Section ======================" << std::endl
		   << std::endl;

	nPlots = 0;
	for(it=plotters_.begin(); it!=plotters_.end(); ++it) {
		if(nPlots++ < maxPlots) {
			result << (*it)->bodyData()
				   << std::endl;
		}
	}

	// Plot all footer data up to the maximum allowed number
	result << "  //===================  Plot Section ======================" << std::endl
		   << std::endl;

	nPlots = 0;
	for(it=plotters_.begin(); it!=plotters_.end(); ++it) {
		if(nPlots < maxPlots) {
			result
				<< "  graphPad->cd(" << nPlots+1 << ");" << std::endl /* cd starts at 1 */
				<< (*it)->footerData()
				<< std::endl;

			nPlots++;
		}
	}

	result
	    << "  graphPad->cd();" << std::endl
		<< "  cc->cd();" << std::endl
		<< "}" << std::endl;

	return result.str();
}

/******************************************************************************/
/**
 * A default header for a ROOT file
 */
std::string GPlotDesigner::staticHeader() const {
	std::ostringstream result;

	result
	  << "  gROOT->Reset();" << std::endl
	  << "  gStyle->SetCanvasColor(0);" << std::endl
	  << "  gStyle->SetStatBorderSize(1);" << std::endl
	  << "  gStyle->SetOptStat(0);" << std::endl
	  << std::endl
	  << "  TCanvas *cc = new TCanvas(\"cc\", \"cc\",0,0,"<< c_x_dim_ << "," << c_y_dim_ << ");" << std::endl
	  << std::endl
	  << "  TPaveLabel* canvasTitle = new TPaveLabel(0.2,0.95,0.8,0.99, \"" << canvasLabel_ << "\");" << std::endl
	  << "  canvasTitle->Draw();" << std::endl
	  << std::endl
	  << "  TPad* graphPad = new TPad(\"Graphs\", \"Graphs\", 0.01, 0.01, 0.99, 0.94);" << std::endl
	  << "  graphPad->Draw();" << std::endl
	  << "  graphPad->Divide(" << c_x_div_ << "," << c_y_div_ << ");" << std::endl
	  << std::endl;

	return result.str();
}

/******************************************************************************/
/**
 * Allows to add a new plotter object
 *
 * @param plotter_ptr A pointer to a plotter
 */
void GPlotDesigner::registerPlotter(boost::shared_ptr<GBasePlotter> plotter_ptr) {
	if(plotter_ptr) {
		plotter_ptr->setId(plotters_.size());
		plotters_.push_back(plotter_ptr);
	} else {
	   glogger
	   << "GPlotDesigner::registerPlotter(): Error!" << std::endl
      << "Got empty plotter" << std::endl
      << GEXCEPTION;
	}
}

/******************************************************************************/
/**
 * Set the dimensions of the output canvas
 *
 * @param c_x_dim The x-dimension of the output canvas
 * @param c_x_dim The y-dimension of the output canvas
 */
void GPlotDesigner::setCanvasDimensions(
	const boost::uint32_t& c_x_dim
	, const boost::uint32_t& c_y_dim
) {
	c_x_dim_ = c_x_dim;
	c_y_dim_ = c_y_dim;
}

/******************************************************************************/
/**
 * Allows to retrieve the canvas dimensions
 *
 * @return A boost::tuple holding the canvas dimensions
 */
boost::tuple<boost::uint32_t, boost::uint32_t> GPlotDesigner::getCanvasDimensions() const {
	return boost::tuple<boost::uint32_t, boost::uint32_t>(c_x_dim_, c_y_dim_);
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */
