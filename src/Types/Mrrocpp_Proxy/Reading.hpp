/*
 * Reading.hpp
 *
 */

#ifndef READING_HPP_
#define READING_HPP_

#include "Common/xdr/xdr_oarchive.hpp"

#include "Logger.hpp"

namespace Types {
namespace Mrrocpp_Proxy {

/**
 * Reading for MRROC++ proxy.
 */
class Reading
{
public:
	Reading()
	{
	}

	virtual ~Reading()
	{
	}

	virtual Reading* clone() = 0;

	/**
	 * Serialize object to archive.
	 * @param ar
	 */
	virtual void send(boost::shared_ptr<xdr_oarchive<> > & ar) = 0;

	/**
	 * Timestamp when processing starts (taken just after camera source).
	 */
	uint64_t processingStartSeconds;
	uint64_t processingStartNanoseconds;

	/**
	 * Timestamp when processing ends (taken just before sending to mrroc proxy).
	 */
	uint64_t processingEndSeconds;
	uint64_t processingEndNanoseconds;
private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		LOG(LWARNING) << "Reading::serialize()\n";
		ar & processingStartSeconds;
		ar & processingStartNanoseconds;

		ar & processingEndSeconds;
		ar & processingEndNanoseconds;
	}
};

}//namespace Mrrocpp_Proxy
}//namespace Types

#endif /* READING_HPP_ */
