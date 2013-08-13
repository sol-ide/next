
//          Copyright Sylvain Oliver 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "thread_group.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

namespace next
{
	class dispatcher;

	class message_handling_thread
	{
	public:
		message_handling_thread( dispatcher& d );
		~message_handling_thread();

        void thread_loop( std::condition_variable& wait_thread_launch, std::mutex& thread_launch_mutex );
        void wake_up_thread( std::shared_ptr< thread_group > group = nullptr  );
        void wait_for_nothing_to_do();
	private:
		void ask_end_thread();
		bool thread_as_not_been_asked_to_end() const;

		void wait_for_message( std::unique_lock< std::mutex >& lock );

	private:
		std::thread thread_;

		dispatcher& dispatcher_;

		bool thread_as_been_asked_to_end_;
		mutable std::mutex thread_as_been_asked_to_end_mutex_;


		std::condition_variable wait_for_message_condition_;
		mutable std::mutex wait_for_message_lock_;

        std::shared_ptr< thread_group > currenlty_handled_group_;
	};
}