#include <common/com_helpers.h>

#include "Helpers.h"

#include <algorithm>
#include <ut/assert.h>
#include <ut/test.h>

using namespace std;

namespace micro_profiler
{
	namespace tests
	{
		begin_test_suite( COMHelpersTests )
			test( InplaceCopyInternalStatisticsToMarshalledStatistics )
			{
				// INIT
				function_statistics s1(1, 2, 3, 5), s2(7, 11, 13, 17);
				function_statistics_detailed s3;
				FunctionStatistics ms1, ms2, ms3;

				s3.times_called = 19;
				s3.max_reentrance = 23;
				s3.inclusive_time = 29;
				s3.exclusive_time = 31;

				// ACT
				copy(make_pair((const void *)123, s1), ms1);
				copy(make_pair((const void *)234, s2), ms2);
				copy(make_pair((const void *)345, s3), ms3);

				// ASSERT
				assert_equal(118, ms1.FunctionAddress);
				assert_equal(1, ms1.TimesCalled);
				assert_equal(2, ms1.MaxReentrance);
				assert_equal(3, ms1.InclusiveTime);
				assert_equal(5, ms1.ExclusiveTime);

				assert_equal(229, ms2.FunctionAddress);
				assert_equal(7, ms2.TimesCalled);
				assert_equal(11, ms2.MaxReentrance);
				assert_equal(13, ms2.InclusiveTime);
				assert_equal(17, ms2.ExclusiveTime);

				assert_equal(340, ms3.FunctionAddress);
				assert_equal(19, ms3.TimesCalled);
				assert_equal(23, ms3.MaxReentrance);
				assert_equal(29, ms3.InclusiveTime);
				assert_equal(31, ms3.ExclusiveTime);
			}


			test( InplaceCopyDetailedInternalToMarshalledStatisticsCopiesBaseStatistics )
			{
				// INIT
				function_statistics_detailed s1, s2;
				FunctionStatisticsDetailed ms1, ms2;
				vector<FunctionStatistics> dummy_children_buffer;

				s1.times_called = 19;
				s1.max_reentrance = 23;
				s1.inclusive_time = 29;
				s1.exclusive_time = 31;
				s1.max_call_time = 15;

				s2.times_called = 1;
				s2.max_reentrance = 3;
				s2.inclusive_time = 5;
				s2.exclusive_time = 7;
				s2.max_call_time = 8;

				// ACT
				copy(make_pair((void *)1123, s1), ms1, dummy_children_buffer);
				copy(make_pair((void *)2234, s2), ms2, dummy_children_buffer);

				// ASSERT
				assert_equal(1118, ms1.Statistics.FunctionAddress);
				assert_equal(19, ms1.Statistics.TimesCalled);
				assert_equal(23, ms1.Statistics.MaxReentrance);
				assert_equal(29, ms1.Statistics.InclusiveTime);
				assert_equal(31, ms1.Statistics.ExclusiveTime);
				assert_equal(15, ms1.Statistics.MaxCallTime);

				assert_equal(2229, ms2.Statistics.FunctionAddress);
				assert_equal(1, ms2.Statistics.TimesCalled);
				assert_equal(3, ms2.Statistics.MaxReentrance);
				assert_equal(5, ms2.Statistics.InclusiveTime);
				assert_equal(7, ms2.Statistics.ExclusiveTime);
				assert_equal(8, ms2.Statistics.MaxCallTime);
			}


			test( InplaceCopyDetailedInternalToMarshalledThrowsIfChildrenBufferIsInsufficient )
			{
				// INIT
				function_statistics_detailed s1, s2;
				FunctionStatisticsDetailed ms;
				vector<FunctionStatistics> children_buffer;

				s1.callees[(void *)123] = function_statistics(10, 20, 30, 40, 50);
				s2.callees[(void *)234] = function_statistics(11, 21, 31, 41, 51);
				s2.callees[(void *)345] = function_statistics(12, 22, 32, 42, 52);

				// ACT / ASSERT
				assert_throws(copy(make_pair((void *)1123, s1), ms, children_buffer), invalid_argument);

				// INIT
				children_buffer.reserve(1);

				// ACT / ASSERT
				assert_throws(copy(make_pair((void *)1123, s2), ms, children_buffer), invalid_argument);

				// INIT
				children_buffer.resize(1);

				// ACT / ASSERT
				assert_throws(copy(make_pair((void *)1123, s1), ms, children_buffer), invalid_argument);
			}


			test( InplaceCopyDetailedInternalToMarshalledStatisticsCopiesChildrenStatistics )
			{
				// INIT
				function_statistics_detailed s0, s2, s3;
				FunctionStatisticsDetailed ms0, ms2, ms3;
				vector<FunctionStatistics> children_buffer;

				children_buffer.reserve(5);
				s2.callees[(void *)123] = function_statistics(10, 20, 30, 40, 50);
				s2.callees[(void *)234] = function_statistics(11, 21, 31, 41, 51);
				s3.callees[(void *)345] = function_statistics(10, 20, 30, 40, 50);
				s3.callees[(void *)456] = function_statistics(11, 21, 31, 41, 51);
				s3.callees[(void *)567] = function_statistics(12, 22, 32, 42, 52);

				// ACT
				copy(make_pair((void *)1123, s0), ms0, children_buffer);
				copy(make_pair((void *)2234, s2), ms2, children_buffer);
				copy(make_pair((void *)3345, s3), ms3, children_buffer);

				// ASSERT
				assert_equal(5u, children_buffer.size());

				assert_equal(0, ms0.ChildrenCount);
				assert_null(ms0.ChildrenStatistics);

				assert_equal(2, ms2.ChildrenCount);
				assert_equal(&children_buffer[0], ms2.ChildrenStatistics);
				sort(ms2.ChildrenStatistics, ms2.ChildrenStatistics + ms2.ChildrenCount, &less_fs);
				assert_equal(118, ms2.ChildrenStatistics[0].FunctionAddress);
				assert_equal(10, ms2.ChildrenStatistics[0].TimesCalled);
				assert_equal(20, ms2.ChildrenStatistics[0].MaxReentrance);
				assert_equal(30, ms2.ChildrenStatistics[0].InclusiveTime);
				assert_equal(40, ms2.ChildrenStatistics[0].ExclusiveTime);
				assert_equal(50, ms2.ChildrenStatistics[0].MaxCallTime);
				assert_equal(229, ms2.ChildrenStatistics[1].FunctionAddress);
				assert_equal(11, ms2.ChildrenStatistics[1].TimesCalled);
				assert_equal(21, ms2.ChildrenStatistics[1].MaxReentrance);
				assert_equal(31, ms2.ChildrenStatistics[1].InclusiveTime);
				assert_equal(41, ms2.ChildrenStatistics[1].ExclusiveTime);
				assert_equal(51, ms2.ChildrenStatistics[1].MaxCallTime);

				assert_equal(3, ms3.ChildrenCount);
				assert_equal(&children_buffer[2], ms3.ChildrenStatistics);
				sort(ms3.ChildrenStatistics, ms3.ChildrenStatistics + ms3.ChildrenCount, &less_fs);
				assert_equal(340, ms3.ChildrenStatistics[0].FunctionAddress);
				assert_equal(10, ms3.ChildrenStatistics[0].TimesCalled);
				assert_equal(20, ms3.ChildrenStatistics[0].MaxReentrance);
				assert_equal(30, ms3.ChildrenStatistics[0].InclusiveTime);
				assert_equal(40, ms3.ChildrenStatistics[0].ExclusiveTime);
				assert_equal(50, ms3.ChildrenStatistics[0].MaxCallTime);
				assert_equal(451, ms3.ChildrenStatistics[1].FunctionAddress);
				assert_equal(11, ms3.ChildrenStatistics[1].TimesCalled);
				assert_equal(21, ms3.ChildrenStatistics[1].MaxReentrance);
				assert_equal(31, ms3.ChildrenStatistics[1].InclusiveTime);
				assert_equal(41, ms3.ChildrenStatistics[1].ExclusiveTime);
				assert_equal(51, ms3.ChildrenStatistics[1].MaxCallTime);
				assert_equal(562, ms3.ChildrenStatistics[2].FunctionAddress);
				assert_equal(12, ms3.ChildrenStatistics[2].TimesCalled);
				assert_equal(22, ms3.ChildrenStatistics[2].MaxReentrance);
				assert_equal(32, ms3.ChildrenStatistics[2].InclusiveTime);
				assert_equal(42, ms3.ChildrenStatistics[2].ExclusiveTime);
				assert_equal(52, ms3.ChildrenStatistics[2].MaxCallTime);
			}


			test( CalculateTotalChildrenStatisticsCount )
			{
				// INIT
				statistics_map_detailed m0, m1, m2;

				m0[(void *)1];

				m1[(void *)1].callees[(void *)123];
				m1[(void *)1].callees[(void *)234];

				m2[(void *)1].callees[(void *)123];
				m2[(void *)1].callees[(void *)234];
				m2[(void *)2];
				m2[(void *)3].callees[(void *)123];
				m2[(void *)3].callees[(void *)234];
				m2[(void *)3].callees[(void *)345];

				// ACT
				size_t count0 = total_children_count(m0.begin(), m0.end());
				size_t count1 = total_children_count(m1.begin(), m1.end());
				size_t count2 = total_children_count(m2.begin(), m2.end());

				// ASSERT
				assert_equal(0u, count0);
				assert_equal(2u, count1);
				assert_equal(5u, count2);
			}


			test( CopyEmptyDetailedStatistics )
			{
				// INIT
				vector<FunctionStatisticsDetailed> buffer1, buffer2(3);
				vector<FunctionStatistics> children_buffer1, children_buffer2(2);
				statistics_map_detailed m;

				// ACT
				copy(m.begin(), m.end(), buffer1, children_buffer1);
				copy(m.begin(), m.end(), buffer2, children_buffer2);

				// ASSERT
				assert_is_empty(buffer1);
				assert_is_empty(children_buffer1);
				assert_is_empty(buffer2);
				assert_is_empty(children_buffer2);
			}


			test( CopyDetailedStatisticsNoChildren )
			{
				// INIT
				vector<FunctionStatisticsDetailed> buffer1, buffer2(3);
				vector<FunctionStatistics> children_buffer1, children_buffer2(2);
				statistics_map_detailed m1, m2;

				m1[(void *)6].times_called = 2;
				m1[(void *)6].max_reentrance = 3;
				m1[(void *)6].inclusive_time = 5;
				m1[(void *)6].exclusive_time = 7;
				m1[(void *)6].max_call_time = 9;
				m1[(void *)16].times_called = 13;
				m1[(void *)16].max_reentrance = 17;
				m1[(void *)16].inclusive_time = 19;
				m1[(void *)16].exclusive_time = 23;
				m1[(void *)16].max_call_time = 27;

				m2[(void *)34].times_called = 31;
				m2[(void *)34].max_reentrance = 37;
				m2[(void *)34].inclusive_time = 41;
				m2[(void *)34].exclusive_time = 43;
				m2[(void *)34].max_call_time = 47;
				m2[(void *)52].times_called = 48;
				m2[(void *)52].max_reentrance = 49;
				m2[(void *)52].inclusive_time = 50;
				m2[(void *)52].exclusive_time = 51;
				m2[(void *)52].max_call_time = 52;
				m2[(void *)57].times_called = 53;
				m2[(void *)57].max_reentrance = 54;
				m2[(void *)57].inclusive_time = 55;
				m2[(void *)57].exclusive_time = 56;
				m2[(void *)57].max_call_time = 57;

				// ACT
				copy(m1.begin(), m1.end(), buffer1, children_buffer1);
				copy(m2.begin(), m2.end(), buffer2, children_buffer2);

				// ASSERT
				assert_equal(2u, buffer1.size());
				sort(buffer1.begin(), buffer1.end(), &less_fsd);
				assert_equal(1, buffer1[0].Statistics.FunctionAddress);
				assert_equal(2, buffer1[0].Statistics.TimesCalled);
				assert_equal(3, buffer1[0].Statistics.MaxReentrance);
				assert_equal(5, buffer1[0].Statistics.InclusiveTime);
				assert_equal(7, buffer1[0].Statistics.ExclusiveTime);
				assert_equal(9, buffer1[0].Statistics.MaxCallTime);
				assert_equal(0, buffer1[0].ChildrenCount);
				assert_equal(11, buffer1[1].Statistics.FunctionAddress);
				assert_equal(13, buffer1[1].Statistics.TimesCalled);
				assert_equal(17, buffer1[1].Statistics.MaxReentrance);
				assert_equal(19, buffer1[1].Statistics.InclusiveTime);
				assert_equal(23, buffer1[1].Statistics.ExclusiveTime);
				assert_equal(27, buffer1[1].Statistics.MaxCallTime);
				assert_equal(0, buffer1[1].ChildrenCount);

				assert_equal(3u, buffer2.size());
				sort(buffer2.begin(), buffer2.end(), &less_fsd);
				assert_equal(29, buffer2[0].Statistics.FunctionAddress);
				assert_equal(31, buffer2[0].Statistics.TimesCalled);
				assert_equal(37, buffer2[0].Statistics.MaxReentrance);
				assert_equal(41, buffer2[0].Statistics.InclusiveTime);
				assert_equal(43, buffer2[0].Statistics.ExclusiveTime);
				assert_equal(47, buffer2[0].Statistics.MaxCallTime);
				assert_equal(0, buffer2[0].ChildrenCount);
				assert_equal(47, buffer2[1].Statistics.FunctionAddress);
				assert_equal(48, buffer2[1].Statistics.TimesCalled);
				assert_equal(49, buffer2[1].Statistics.MaxReentrance);
				assert_equal(50, buffer2[1].Statistics.InclusiveTime);
				assert_equal(51, buffer2[1].Statistics.ExclusiveTime);
				assert_equal(52, buffer2[1].Statistics.MaxCallTime);
				assert_equal(0, buffer2[1].ChildrenCount);
				assert_equal(52, buffer2[2].Statistics.FunctionAddress);
				assert_equal(53, buffer2[2].Statistics.TimesCalled);
				assert_equal(54, buffer2[2].Statistics.MaxReentrance);
				assert_equal(55, buffer2[2].Statistics.InclusiveTime);
				assert_equal(56, buffer2[2].Statistics.ExclusiveTime);
				assert_equal(57, buffer2[2].Statistics.MaxCallTime);
				assert_equal(0, buffer2[2].ChildrenCount);
			}


			test( CopyDetailedStatisticsWithChildren )
			{
				// INIT
				vector<FunctionStatisticsDetailed> buffer1, buffer2(7);
				vector<FunctionStatistics> children_buffer1, children_buffer2(9);
				statistics_map_detailed m1, m2;

				m1[(void *)1].callees[(void *)123] = function_statistics(1, 2, 3, 4, 5);
				m1[(void *)1].callees[(void *)234] = function_statistics(5, 6, 7, 8, 9);

				m2[(void *)1].callees[(void *)123] = function_statistics(9, 10, 11, 12, 13);
				m2[(void *)1].callees[(void *)234] = function_statistics(13, 14, 15, 16, 17);
				m2[(void *)2];
				m2[(void *)3].callees[(void *)123] = function_statistics(17, 18, 19, 20, 21);

				// ACT
				copy(m1.begin(), m1.end(), buffer1, children_buffer1);
				copy(m2.begin(), m2.end(), buffer2, children_buffer2);

				// ASSERT
				assert_equal(2u, children_buffer1.size());
				assert_equal(3u, children_buffer2.size());
				sort(buffer2.begin(), buffer2.end(), &less_fsd);

				assert_equal(2, buffer1[0].ChildrenCount);
				assert_not_null(buffer1[0].ChildrenStatistics);
				assert_equal(2, buffer2[0].ChildrenCount);
				assert_not_null(buffer2[0].ChildrenStatistics);
				assert_equal(0, buffer2[1].ChildrenCount);
				assert_null(buffer2[1].ChildrenStatistics);
				assert_equal(1, buffer2[2].ChildrenCount);
				assert_not_null(buffer2[2].ChildrenStatistics);
			}


			test( InplaceAdditionReturnsNonConstRefToLHS )
			{
				// INIT
				function_statistics destination(13, 0, 50000700001, 13002, 0);
				FunctionStatistics addendum = {	0, 19, 0, 23, 31, 0 };

				// ACT
				const function_statistics *result = &(destination += addendum);

				// ASSERT
				assert_equal(&destination, result);
			}


			test( InplaceAddingStatisticsToInternalAddsTimesCalledAndInclusiveExclusiveTimes )
			{
				// INIT
				function_statistics destination[] = {
					function_statistics(0, 0, 0, 0, 0),
					function_statistics(1, 0, 10001, 3002, 0),
					function_statistics(13, 0, 50000700001, 13002, 0),
					function_statistics(131, 0, 12345678, 345678, 0),
				};
				FunctionStatistics addendum[] = {
					// address, times called, max reentrance, exclusive time, inclusive time, max call time
					{	0, 3, 0, 5, 7, 0 },
					{	0, 11, 0, 13, 17, 0 },
					{	0, 19, 0, 23, 31, 0 },
					{	0, 37, 0, 41, 47, 0 },
				};

				// ACT
				destination[0] += addendum[0];
				destination[1] += addendum[1];
				destination[2] += addendum[2];
				destination[3] += addendum[3];

				// ASSERT
				assert_equal(function_statistics(3, 0, 7, 5, 0), destination[0]);
				assert_equal(function_statistics(12, 0, 10018, 3015, 0), destination[1]);
				assert_equal(function_statistics(32, 0, 50000700032, 13025, 0), destination[2]);
				assert_equal(function_statistics(168, 0, 12345725, 345719, 0), destination[3]);
			}


			test( InplaceAddingStatisticsToInternalUpdatesMaximumValuesForReentranceAndMaxCallTime )
			{
				// INIT
				function_statistics destination[] = {
					function_statistics(0, 0, 0, 0, 100),
					function_statistics(0, 4, 0, 0, 70),
					function_statistics(0, 9, 0, 0, 5),
					function_statistics(0, 13, 0, 0, 0),
				};
				FunctionStatistics addendum[] = {
					// address, times called, max reentrance, exclusive time, inclusive time, max call time
					{	0, 1, 3, 10, 21, 0 },
					{	0, 2, 5, 11, 22, 70 },
					{	0, 3, 9, 12, 23, 6 },
					{	0, 4, 10, 13, 24, 9 },
				};

				// ACT
				destination[0] += addendum[0];
				destination[1] += addendum[1];
				destination[2] += addendum[2];
				destination[3] += addendum[3];

				// ASSERT
				assert_equal(function_statistics(1, 3, 21, 10, 100), destination[0]);
				assert_equal(function_statistics(2, 5, 22, 11, 70), destination[1]);
				assert_equal(function_statistics(3, 9, 23, 12, 6), destination[2]);
				assert_equal(function_statistics(4, 13, 24, 13, 9), destination[3]);
			}


			test( InplaceAdditionDetailedReturnsNonConstRefToLHS )
			{
				// INIT
				function_statistics_detailed destination;
				FunctionStatisticsDetailed addendum = {	{	0, 19, 0, 23, 31, 0 }, 0, 0	};

				// ACT
				const function_statistics_detailed *result = &(destination += addendum);

				// ASSERT
				assert_equal(&destination, result);
			}


			test( InplaceAddingDetailedStatisticsToInternalNoChildrenUpdates )
			{
				// INIT
				function_statistics_detailed destination[] = {
					function_statistics_ex(0, 0, 0, 0, 100),
					function_statistics_ex(0, 4, 0, 0, 70),
					function_statistics_ex(0, 9, 0, 0, 5),
					function_statistics_ex(0, 13, 0, 0, 0),
				};
				FunctionStatisticsDetailed addendum[] = {
					{	{	0, 1, 3, 10, 21, 0 }, 0, 0	},
					{	{	0, 2, 5, 11, 22, 70 }, 0, 0	},
					{	{	0, 3, 9, 12, 23, 6 }, 0, 0	},
					{	{	0, 4, 10, 13, 24, 9 }, 0, 0	},
				};

				// ACT
				destination[0] += addendum[0];
				destination[1] += addendum[1];
				destination[2] += addendum[2];
				destination[3] += addendum[3];

				// ASSERT
				assert_equal(function_statistics(1, 3, 21, 10, 100), destination[0]);
				assert_equal(function_statistics(2, 5, 22, 11, 70), destination[1]);
				assert_equal(function_statistics(3, 9, 23, 12, 6), destination[2]);
				assert_equal(function_statistics(4, 13, 24, 13, 9), destination[3]);
			}


			test( InplaceAddingDetailedStatisticsToInternalWithChildrenUpdates1 )
			{
				// INIT
				function_statistics_detailed destination[2];
				FunctionStatistics children[] = {
					{	0x00001234, 1, 5, 9, 13, 0 },
					{	0x00012340, 2, 6, 10, 14, 17 },
					{	0x00123400, 3, 7, 11, 15, 18 },
					{	0x01234000, 4, 8, 12, 16, 19 },
				};
				FunctionStatisticsDetailed addendum[] = {
					{	{	0	}, 1, &children[0]	},
					{	{	0	}, 3, &children[1]	},
				};

				// ACT
				destination[0] += addendum[0];
				destination[1] += addendum[1];

				// ASSERT
				assert_equal(1u, destination[0].callees.size());
				assert_equal(function_statistics(1, 5, 13, 9, 0), destination[0].callees[(void *)0x00001234]);

				assert_equal(3u, destination[1].callees.size());
				assert_equal(function_statistics(2, 6, 14, 10, 17), destination[1].callees[(void *)0x00012340]);
				assert_equal(function_statistics(3, 7, 15, 11, 18), destination[1].callees[(void *)0x00123400]);
				assert_equal(function_statistics(4, 8, 16, 12, 19), destination[1].callees[(void *)0x01234000]);
			}


			test( InplaceAddingDetailedStatisticsToInternalWithChildrenUpdates2 )
			{
				// INIT
				function_statistics_detailed destination;
				FunctionStatistics children[] = {
					{	0x00001234, 1, 5, 9, 13, 0 },
					{	0x00012340, 2, 6, 10, 14, 29 },
				};
				FunctionStatisticsDetailed addendum = {	{	0	}, 2, &children[0]	};

				destination.callees[(void *)0x00012340] = function_statistics(3, 7, 15, 11, 18);
				destination.callees[(void *)0x00123400] = function_statistics(4, 8, 16, 12, 19);

				// ACT
				destination += addendum;

				// ASSERT
				assert_equal(3u, destination.callees.size());
				assert_equal(function_statistics(1, 5, 13, 9, 0), destination.callees[(void *)0x00001234]);
				assert_equal(function_statistics(5, 7, 29, 21, 29), destination.callees[(void *)0x00012340]);
				assert_equal(function_statistics(4, 8, 16, 12, 19), destination.callees[(void *)0x00123400]);
			}
		end_test_suite
	}
}
