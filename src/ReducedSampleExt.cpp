#include "interface/ReducedSampleExt.h"
#include <vector>
#include <stdexcept>
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "L1Trigger/MenuGeneration/interface/l1menu/ReducedEvent.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/FullSample.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/TriggerMenu.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/ITrigger.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/ICachedTrigger.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/L1TriggerDPGEvent.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/IEvent.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/IMenuRate.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/ITriggerRate.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/tools/miscellaneous.h"
#include "L1Trigger/MenuGeneration/src/implementation/MenuRateImplementation.h"
#include "L1Trigger/MenuGeneration/src/protobuf/l1menu.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>

l1menu::ReducedSampleExt::ReducedSampleExt():
	menuFilename_("GiveMeATriggerMenu.txt"),
	protobufFilename_("output.pb")
{
	init();
}

l1menu::ReducedSampleExt::ReducedSampleExt(const std::string& menuFilename, const std::string& protobufFilename):
	menuFilename_(menuFilename),
	protobufFilename_(protobufFilename)
{
	init();
}

l1menu::ReducedSample::init()
{
	// common block to do initialisations 
	// - get trigger menu
	// - setup protobuf file

	///////////////////////
	// Get trigger menu //
	///////////////////////
	pMyMenu = l1menu::tools::loadMenu( menuFilename_ );

	////////////////////////////
	// Setup protobuf output //
	////////////////////////////
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// I need to copy the details of the trigger menu into the protobuf storage.
	// This means I'm holding a duplicate, but I need it to write the sample to a
	// protobuf file, so I might as well do it now.
	for( size_t triggerNumber=0; triggerNumber<triggerMenu.numberOfTriggers(); ++triggerNumber )
	{
		const l1menu::ITrigger& trigger=triggerMenu.getTrigger(triggerNumber);

		l1menuprotobuf::Trigger* pProtobufTrigger=protobufSampleHeader.add_trigger();
		pProtobufTrigger->set_name( trigger.name() );
		pProtobufTrigger->set_version( trigger.version() );

		// Record all of the parameters. It's not strictly necessary to record the values
		// of the parameters that are recorded for each event, but I might as well so that
		// the trigger menu is loaded exactly as it was saved.
		const auto parameterNames=trigger.parameterNames();
		for( const auto& parameterName : parameterNames )
		{
			l1menuprotobuf::Trigger_TriggerParameter* pProtobufParameter=pProtobufTrigger->add_parameter();
			pProtobufParameter->set_name(parameterName);
			pProtobufParameter->set_value( trigger.parameter(parameterName) );
		}

		// Make a note of the names of the parameters that are recorded for each event. For this
		// I'm just recording the parameters that refer to the thresholds.
		const auto thresholdNames=l1menu::tools::getThresholdNames(trigger);
		for( const auto& thresholdName : thresholdNames ) pProtobufTrigger->add_varying_parameter(thresholdName);

	} // end of loop over triggers

	// Always make sure there is at least one Run ready to be added to
	std::unique_ptr<l1menuprotobuf::Run> pNewRun( new l1menuprotobuf::Run );
	protobufRuns.push_back( std::move( pNewRun ) );


}