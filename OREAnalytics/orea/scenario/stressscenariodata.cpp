/*
 Copyright (C) 2017 Quaternion Risk Management Ltd
 All rights reserved.

 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

#include <orea/scenario/stressscenariodata.hpp>
#include <ored/utilities/log.hpp>
#include <ored/utilities/xmlutils.hpp>

using namespace QuantLib;
using namespace std;

namespace ore {
namespace analytics {

void StressTestScenarioData::fromXML(XMLNode* root) {
    data_.clear();

    XMLNode* node = XMLUtils::locateNode(root, "StressTesting");
    XMLUtils::checkNode(node, "StressTesting");

    for (XMLNode* testCase = XMLUtils::getChildNode(node, "StressTest"); testCase;
         testCase = XMLUtils::getNextSibling(testCase)) {

        StressTestData test;
        test.label = XMLUtils::getAttribute(testCase, "id");
        // XMLUtils::getChildValue(testCase, "Label", true);

        LOG("Load stress test label " << test.label);

        LOG("Get discount curve shift parameters");
        XMLNode* discountCurves = XMLUtils::getChildNode(testCase, "DiscountCurves");
        if (discountCurves)
		{
			test.discountCurveShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(discountCurves, "DiscountCurve"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string ccy = XMLUtils::getAttribute(child, "ccy");
				LOG("Loading stress parameters for discount curve for ccy " << ccy);
				CurveShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
				data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
				data.shiftTenors = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftTenors", true);
				QL_REQUIRE(data.shifts.size() == data.shiftTenors.size(),
						   "number of tenors and shifts does not match in discount curve stress data");
				QL_REQUIRE(data.shifts.size() > 0, "no shifts provided in discount curve stress data");
				test.discountCurveShifts[ccy] = data;
			}
		}	
        LOG("Get index curve stress parameters");
        XMLNode* indexCurves = XMLUtils::getChildNode(testCase, "IndexCurves");
        if (indexCurves) 
		{
			test.indexCurveShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(indexCurves, "IndexCurve"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string index = XMLUtils::getAttribute(child, "index");
				LOG("Loading stress parameters for index " << index);
				// same as discount curve sensitivity loading from here ...
				CurveShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
				data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
				data.shiftTenors = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftTenors", true);
				QL_REQUIRE(data.shifts.size() == data.shiftTenors.size(),
						   "number of tenors and shifts does not match in index curve stress data");
				QL_REQUIRE(data.shifts.size() > 0, "no shifts provided in index curve stress data");
				test.indexCurveShifts[index] = data;
			}
		}

        LOG("Get yield curve stress parameters");
        XMLNode* yieldCurves = XMLUtils::getChildNode(testCase, "YieldCurves");
        if (yieldCurves)
		{
			QL_REQUIRE(yieldCurves, "YieldCurves node not found");
			test.yieldCurveShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(yieldCurves, "YieldCurve"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string name = XMLUtils::getAttribute(child, "name");
				LOG("Loading stress parameters for yield curve " << name);
				// same as discount curve sensitivity loading from here ...
				CurveShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
				data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
				data.shiftTenors = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftTenors", true);
				QL_REQUIRE(data.shifts.size() == data.shiftTenors.size(),
						   "number of tenors and shifts does not match in yield curve stress data");
				QL_REQUIRE(data.shifts.size() > 0, "no shifts provided in yield curve stress data");
				test.yieldCurveShifts[name] = data;
			}
		}
        LOG("Get FX spot stress parameters");
        XMLNode* fxSpots = XMLUtils::getChildNode(testCase, "FxSpots");
        if (fxSpots)
		{
			QL_REQUIRE(fxSpots, "FxSpots node not found");
			test.fxShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(fxSpots, "FxSpot"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string ccypair = XMLUtils::getAttribute(child, "ccypair");
				LOG("Loading stress parameters for FX " << ccypair);
				SpotShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
				data.shiftSize = XMLUtils::getChildValueAsDouble(child, "ShiftSize", true);
				test.fxShifts[ccypair] = data;
			}
		}
        LOG("Get fx vol stress parameters");
        XMLNode* fxVols = XMLUtils::getChildNode(testCase, "FxVolatilities");
        if (fxVols)
		{
			QL_REQUIRE(fxVols, "FxVols node not found");
			test.fxVolShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(fxVols, "FxVolatility"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string ccypair = XMLUtils::getAttribute(child, "ccypair");
				LOG("Loading stress parameters for FX vols " << ccypair);
				VolShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType");
				data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
				data.shiftExpiries = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftExpiries", true);
				test.fxVolShifts[ccypair] = data;
			}
		}
        LOG("Get Equity spot stress parameters");
        XMLNode* equitySpots = XMLUtils::getChildNode(testCase, "EquitySpots");
        if (equitySpots)
		{
			QL_REQUIRE(equitySpots, "EquitySpots node not found");
			test.equityShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(equitySpots, "EquitySpot"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string equity = XMLUtils::getAttribute(child, "equity");
				LOG("Loading stress parameters for Equity " << equity);
				SpotShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
				data.shiftSize = XMLUtils::getChildValueAsDouble(child, "ShiftSize", true);
				test.equityShifts[equity] = data;
			}
		}
        LOG("Get equity vol stress parameters");
        XMLNode* equityVols = XMLUtils::getChildNode(testCase, "EquityVolatilities");
        if (equityVols)
		{
			QL_REQUIRE(equityVols, "FxVols node not found");
			test.equityVolShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(equityVols, "EquityVolatility"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string equity = XMLUtils::getAttribute(child, "equity");
				LOG("Loading stress parameters for Equity vols " << equity);
				VolShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType");
				data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
				data.shiftExpiries = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftExpiries", true);
				test.equityVolShifts[equity] = data;
			}
		}
        LOG("Get swaption vol stress parameters");
        XMLNode* swaptionVols = XMLUtils::getChildNode(testCase, "SwaptionVolatilities");
        if (swaptionVols)
		{
			QL_REQUIRE(swaptionVols, "SwaptionVols node not found");
			test.swaptionVolShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(swaptionVols, "SwaptionVolatility"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string ccy = XMLUtils::getAttribute(child, "ccy");
				LOG("Loading stress parameters for swaption vols " << ccy);
				SwaptionVolShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
				data.shiftTerms = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftTerms", true);
				data.shiftExpiries = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftExpiries", true);
				XMLNode* shiftSizes = XMLUtils::getChildNode(child, "Shifts");
				data.parallelShiftSize = 0.0;
				for (XMLNode* child2 = XMLUtils::getChildNode(shiftSizes, "Shift"); child2;
					 child2 = XMLUtils::getNextSibling(child2)) {
					string expiry = XMLUtils::getAttribute(child2, "expiry");
					string term = XMLUtils::getAttribute(child2, "term");
					if (expiry == "" && term == "")
						data.parallelShiftSize = ore::data::parseReal(XMLUtils::getNodeValue(child2));
					else {
						QL_REQUIRE(expiry != "" && term != "", "expiry and term attributes required on shift size nodes");
						Period e = ore::data::parsePeriod(expiry);
						Period t = ore::data::parsePeriod(term);
						Real value = ore::data::parseReal(XMLUtils::getNodeValue(child2));
						pair<Period, Period> key(e, t);
						data.shifts[key] = value;
					}
				}
				test.swaptionVolShifts[ccy] = data;
			}
		}
        LOG("Get cap/floor vol stress parameters");
        XMLNode* capVols = XMLUtils::getChildNode(testCase, "CapFloorVolatilities");
        if (capVols)
		{
			QL_REQUIRE(capVols, "CapVols node not found");
			for (XMLNode* child = XMLUtils::getChildNode(capVols, "CapFloorVolatility"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string ccy = XMLUtils::getAttribute(child, "ccy");
				CapFloorVolShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
				data.shiftExpiries = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftExpiries", true);
				data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
				test.capVolShifts[ccy] = data;
			}
		}
        LOG("Get Security spread stress parameters");
        XMLNode* securitySpreads = XMLUtils::getChildNode(testCase, "SecuritySpreads");
        if (securitySpreads)
		{
			QL_REQUIRE(securitySpreads, "SecuritySpreads node not found");
			test.securitySpreadShifts.clear();
			for (XMLNode* child = XMLUtils::getChildNode(securitySpreads, "SecuritySpread"); child;
				 child = XMLUtils::getNextSibling(child)) {
				string bond = XMLUtils::getAttribute(child, "security");
				LOG("Loading stress parameters for Security spreads " << bond);
				SpotShiftData data;
				data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
				data.shiftSize = XMLUtils::getChildValueAsDouble(child, "ShiftSize", true);
				test.securitySpreadShifts[bond] = data;
			}
		}
        data_.push_back(test);

        LOG("Loading stress test label " << test.label << " done");
    }

    LOG("Loading stress tests done");
}

template <typename ShiftT> void dataToXMLCurve(ore::data::XMLDocument & doc, ore::data::XMLNode *node, 
	ShiftT shiftDictionary,
	std::string scenarioLabel,
	std::string identifierAttribute,
	std::string marketDataNames, std::string marketDataName, 
	std::string logMessage)
{
    LOG(logMessage);
    XMLNode* discountCurves = XMLUtils::addChild(doc, node, marketDataNames);
    for (auto aDiscCurveItr = shiftDictionary.begin(); aDiscCurveItr != shiftDictionary.end(); ++aDiscCurveItr) {
        std::string key = aDiscCurveItr->first;
        XMLNode* aDiscCurveNode = XMLUtils::addChild(doc, discountCurves, marketDataName);
        XMLUtils::addAttribute(doc, aDiscCurveNode, identifierAttribute, key);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftType", aDiscCurveItr->second.shiftType);
        XMLUtils::addChild(doc, aDiscCurveNode, "Shifts", aDiscCurveItr->second.shifts);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftTenors", aDiscCurveItr->second.shiftTenors);
    }

}

template <typename ShiftT>
void dataToXMLVol(ore::data::XMLDocument& doc, ore::data::XMLNode* node, ShiftT shiftDictionary,
                    std::string scenarioLabel, std::string identifierAttribute, std::string marketDataNames,
                    std::string marketDataName, std::string logMessage) {
    LOG(logMessage);
    XMLNode* discountCurves = XMLUtils::addChild(doc, node, marketDataNames);
    for (auto aDiscCurveItr = shiftDictionary.begin(); aDiscCurveItr != shiftDictionary.end(); ++aDiscCurveItr) {
        std::string key = aDiscCurveItr->first;
        XMLNode* aDiscCurveNode = XMLUtils::addChild(doc, discountCurves, marketDataName);
        XMLUtils::addAttribute(doc, aDiscCurveNode, identifierAttribute, key);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftType", aDiscCurveItr->second.shiftType);
        XMLUtils::addChild(doc, aDiscCurveNode, "Shifts", aDiscCurveItr->second.shifts);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftExpiries", aDiscCurveItr->second.shiftExpiries);
    }
}


template <typename ShiftT>
void dataToXMLSpot(ore::data::XMLDocument& doc, ore::data::XMLNode* node, ShiftT shiftDictionary,
                  std::string scenarioLabel, std::string identifierAttribute, std::string marketDataNames,
                  std::string marketDataName, std::string logMessage) {

    LOG(logMessage);
    XMLNode* discountCurves = XMLUtils::addChild(doc, node, marketDataNames);
    for (auto aDiscCurveItr = shiftDictionary.begin(); aDiscCurveItr != shiftDictionary.end(); ++aDiscCurveItr) {
        std::string key = aDiscCurveItr->first;
        XMLNode* aDiscCurveNode = XMLUtils::addChild(doc, discountCurves, marketDataName);
        XMLUtils::addAttribute(doc, aDiscCurveNode, identifierAttribute, key);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftType", aDiscCurveItr->second.shiftType);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftSize", aDiscCurveItr->second.shiftSize);        
    }
}

template <typename ShiftT>
void dataToXMLVolCube(ore::data::XMLDocument& doc, ore::data::XMLNode* node, ShiftT shiftDictionary,
                   std::string scenarioLabel, std::string identifierAttribute, std::string marketDataNames,
                   std::string marketDataName, std::string logMessage) {

    LOG(logMessage);
    XMLNode* discountCurves = XMLUtils::addChild(doc, node, marketDataNames);

    for (auto aDiscCurveItr = shiftDictionary.begin(); aDiscCurveItr != shiftDictionary.end(); ++aDiscCurveItr) {
        std::string key = aDiscCurveItr->first;
        XMLNode* aDiscCurveNode = XMLUtils::addChild(doc, discountCurves, marketDataName);
        XMLUtils::addAttribute(doc, aDiscCurveNode, identifierAttribute, key);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftType", aDiscCurveItr->second.shiftType);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftTerms", aDiscCurveItr->second.shiftTerms);
        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftExpiries", aDiscCurveItr->second.shiftExpiries);

		XMLNode* aShiftsNode = XMLUtils::addChild(doc, discountCurves, "Shifts");        
        XMLUtils::addChild(doc, aShiftsNode, "Shift", aDiscCurveItr->second.parallelShiftSize);
		for (auto aShiftItr = aDiscCurveItr->second.shifts.begin(); aShiftItr != aDiscCurveItr->second.shifts.end(); ++aSiftItr)
		{
			XMLNode* aShiftNode = XMLUtils::addChild(doc, aShiftsNode, "Shift");			
			XMLUtils::addAttribute(doc, aShiftNode, "expiry", aShiftItr.first.first);
			XMLUtils::addAttribute(doc, aShiftNode, "term", aShiftItr.first.second);		
			aShiftNode->value(aShiftItr.second);
		}
    }
}




XMLNode* StressTestScenarioData::toXML(ore::data::XMLDocument& doc) {
    XMLNode* node = doc.allocNode("StressTesting");
    for (auto aStressItr = data_.begin(); aStressItr != data_.end(); aStressItr++) {
        /*
                XMLNode *aStressTest = XMLUtils::addChild(doc, node, "StressTest");
                XMLUtils::addAttribute(doc, aStressTest, "id", aStressItr->label);

                LOG("Get discount curve shift parameters");
                XMLNode* discountCurves = XMLUtils::addChild(doc, aStressTest, "DiscountCurves");
                for (auto aDiscCurveItr = aStressItr->discountCurveShifts.begin(); aDiscCurveItr !=
           aStressItr->discountCurveShifts.end(); ++aDiscCurveItr)
                {
                        std::string ccy = aDiscCurveItr->first;
                        XMLNode *aDiscCurveNode = XMLUtils::addChild(doc, discountCurves, "DiscountCurve");
                        XMLUtils::addAttribute(doc, aDiscCurveNode, "ccy", ccy);
                        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftType", aDiscCurveItr->second.shiftType);
                        XMLUtils::addChild(doc, aDiscCurveNode, "Shifts", aDiscCurveItr->second.shifts);
                        XMLUtils::addChild(doc, aDiscCurveNode, "ShiftTenors", aDiscCurveItr->second.shiftTenors);
                }
                */
        XMLNode* aStressTest = XMLUtils::addChild(doc, node, "StressTest");
        XMLUtils::addAttribute(doc, aStressTest, "id", aStressItr->label);
        dataToXMLCurve(doc, aStressTest, aStressItr->discountCurveShifts, aStressItr->label, "ccy", "DiscountCurves",
                  "DiscountCurve", "Set discount curve shift parameters");
        dataToXMLCurve(doc, aStressTest, aStressItr->indexCurveShifts, aStressItr->label, "ccy", "IndexCurves",
                  "IndexCurve", "Set index curve shift parameters");
        dataToXMLCurve(doc, aStressTest, aStressItr->yieldCurveShifts, aStressItr->label, "name", "YieldCurves",
                       "YieldCurve",
                  "Set yield curve shift parameters");
        dataToXMLSpot(doc, aStressTest, aStressItr->fxShifts, aStressItr->label, "ccypair", "FxSpots", "FxSpot",
                  "Set FX spot stress parameters");
        dataToXMLVol(doc, aStressTest, aStressItr->fxVolShifts, aStressItr->label, "ccypair", "FxVolatilities",
                     "FxVolatility",
                  "Set FX vol stress parameters");
        dataToXMLSpot(doc, aStressTest, aStressItr->equityShifts, aStressItr->label, "equity", "EquitySpots",
                      "EquitySpot",
                  "Set Equity spot stress parameters");
        dataToXMLVol(doc, aStressTest, aStressItr->equityVolShifts, aStressItr->label, "equity", "EquityVolatilities",
                  "EquityVolatility", "Set Equity vol stress parameters");
        dataToXMLSpot(doc, aStressTest, aStressItr->securitySpreadShifts, aStressItr->label, "security",
                      "SecuritySpreads",
                  "SecuritySpread", "Set Security spread stress parameters");
        dataToXMLVol(doc, aStressTest, aStressItr->capVolShifts, aStressItr->label, "ccy", "CapFloorVolatilities",
                     "CapFloorVolatility", "Set cap/floor vol stress parameters");



    };
    return node;
}
} // namespace analytics
} // namespace ore
