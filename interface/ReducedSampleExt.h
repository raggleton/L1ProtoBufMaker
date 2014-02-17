#ifndef l1menu_ReducedSampleExt_h
#define l1menu_ReducedSampleExt_h

#include "L1Trigger/MenuGeneration/interface/l1menu/ReducedSample.h"
#include "L1Trigger/MenuGeneration/interface/l1menu/TriggerMenu.h"
#include "L1Trigger/MenuGeneration/src/protobuf/l1menu.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>

namespace l1menu
{

	/**
	 * @brief Extension of l1menu::ReducedSample to allow input directly in cmsRun config
	 * 
	 * @details Main addition is an addSample(L1TriggerDPGEventExt) method that takes a L1TriggerDPGEventExt object, 
	 * which is constructed directly from the (processed) AOD. This avoids having to make a FullSample.
	 */			
	class ReducedSampleExt : public l1menu::ReducedSample
	{
	public:
		ReducedSampleExt();
		ReducedSampleExt(const std::string& menuFilename, const std::string& protobufFilename);
		virtual ~ReducedSampleExt();

		void setTriggerMenu(const std::string& menuFilename);

		// Implement the 

	protected:

	private:
		void init(); // For initialising constructors

	    std::unique_ptr<l1menu::TriggerMenu> pMyMenu; // trigger menu, as set by user
    	std::string menuFilename_;
    	std::string protobufFilename_;


	}; // end of class decl

} // end of namespace

#endif