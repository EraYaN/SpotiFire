// User.h

#pragma once
#include "Stdafx.h"

using namespace System;
using namespace System::Collections::Generic;

namespace SpotiFire {

	///-------------------------------------------------------------------------------------------------
	/// <summary>	User. </summary>
	///
	/// <remarks>	Aleksander, 03.02.2013. </remarks>
	///-------------------------------------------------------------------------------------------------
	public ref class User sealed : ISpotifyObject, IAsyncLoaded
	{
	internal:
		Session ^_session;
		sp_user *_ptr;

		User(Session ^session, sp_user *ptr);
		!User(); // finalizer
		~User(); // destructor

	public:

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the session. </summary>
		///
		/// <value>	The session. </value>
		///-------------------------------------------------------------------------------------------------
		virtual property Session ^Session { SpotiFire::Session ^get() sealed; }

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the canonical username. </summary>
		///
		/// <value>	A string representing the canonical username. </value>
		///-------------------------------------------------------------------------------------------------
		virtual property String ^CanonicalName { String ^get() sealed; }

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the display username. </summary>
		///
		/// <value>	A string representing the display username. </value>
		///-------------------------------------------------------------------------------------------------
		virtual property String ^DisplayName { String ^get() sealed; }

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets a value indicating whether this object is loaded. </summary>
		///
		/// <value>	true if this object is loaded, false if not. </value>
		///-------------------------------------------------------------------------------------------------
		virtual property bool IsLoaded { bool get() sealed; }

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets a value indicating whether this object is ready. </summary>
		///
		/// <value>	true if this object is ready, false if not. </value>
		///-------------------------------------------------------------------------------------------------
		virtual property bool IsReady { bool get() sealed; }
	};
}
