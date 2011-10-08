#pragma once
#ifndef _GameTime__H
#define _GameTime__H

namespace Engine
{
	/// <summary>
	/// Contains the current timing state of the game.
	/// </summary>
	class GameTime
	{
	public:

		/// <summary>
		/// Initializes a new instance of the <see cref="GameTimer"/> class.
		/// </summary>
		GameTime(void) {};

		/// <summary>
		/// Initializes a new instance of the <see cref="GameTimer"/> class.
		/// </summary>
		/// <param name="totalRealTime">The total real time.</param>
		/// <param name="elapsedRealTime">The elapsed real time.</param>
		/// <param name="totalGameTime">The total game time.</param>
		/// <param name="elapsedGameTime">The elapsed game time.</param>
		GameTime(float totalRealTime, float elapsedRealTime, float totalGameTime, float elapsedGameTime,
			float fps, bool runningSlowly)
		{
			mTotalRealTime = totalRealTime;
			mElapsedRealTime = elapsedRealTime;
			mTotalGameTime = totalGameTime;
			mElapsedGameTime = elapsedGameTime;
			mFramesPerSecond = fps;
			mIsRunningSlowly = runningSlowly;
		}

		~GameTime(void) {};


		/// <summary>
		/// Gets the current frames-per-second measure.
		/// </summary>
		/// <value>The current frames-per-second measure.<alue>
		float GetFramesPerSecond() const { return mFramesPerSecond; }

		/// <summary>
		/// Gets the elapsed game time, in seconds.
		/// </summary>
		/// <value>The elapsed game time.<alue>
		float GetElapsedGameTime() const { return mElapsedGameTime; }

		/// <summary>
		/// Gets the elapsed real time, in seconds.
		/// </summary>
		/// <value>The elapsed real time.<alue>
		float GetElapsedRealTime() const { return mElapsedRealTime; }

		/// <summary>
		/// Gets the total game time, in seconds.
		/// </summary>
		/// <value>The total game time.<alue>
		float GetTotalGameTime() const { return mTotalGameTime; }


		/// <summary>
		/// Gets the total real time, in seconds.
		/// </summary>
		/// <value>The total real time.<alue>
		float GetTotalRealTime() const { return mTotalRealTime; }

		/// <summary>
		/// Gets fps
		/// </summary>
		float GetFps() const { return mFramesPerSecond; }

		/// <summary>
		/// Gets or sets a value indicating whether this instance is running slowly.
		/// </summary>
		/// <value>
		/// <c>true</c> if this instance is running slowly; otherwise, <c>false</c>.
		/// <alue>
		bool IsRunningSlowly() const { return mIsRunningSlowly; }


	private:

		float mFramesPerSecond;
		float mElapsedGameTime;
		float mElapsedRealTime;
		float mTotalGameTime;
		float mTotalRealTime;

		bool mIsRunningSlowly;

	};
}



#endif // _GameTime__H