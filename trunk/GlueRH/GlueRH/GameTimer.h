#ifndef _GameTimer__H
#define _GameTimer__H

namespace GlueRH
{
	/// <summary>
	/// Contains the current timing state of the game.
	/// </summary>
	class GameTimer
	{
	public:

		/// <summary>
		/// Initializes a new instance of the <see cref="GameTimer"/> class.
		/// </summary>
		GameTimer(void);

		/// <summary>
		/// Initializes a new instance of the <see cref="GameTimer"/> class.
		/// </summary>
		/// <param name="totalRealTime">The total real time.</param>
		/// <param name="elapsedRealTime">The elapsed real time.</param>
		/// <param name="totalGameTime">The total game time.</param>
		/// <param name="elapsedGameTime">The elapsed game time.</param>
		GameTimer(float totalRealTime, float elapsedRealTime, float totalGameTime, float elapsedGameTime,
					float fps, bool runningSlowly)
		{
			mTotalRealTime = totalRealTime;
			mElapsedRealTime = elapsedRealTime;
			mTotalGameTime = totalGameTime;
			mElapsedGameTime = elapsedGameTime;
			mFramesPerSecond = fps;
			mIsRunningSlowly = runningSlowly;
		}
		
		~GameTimer(void);


		/// <summary>
		/// Gets the current frames-per-second measure.
		/// </summary>
		/// <value>The current frames-per-second measure.</value>
		float GetFramesPerSecond() const { return mFramesPerSecond; }

		/// <summary>
		/// Gets the elapsed game time, in seconds.
		/// </summary>
		/// <value>The elapsed game time.</value>
		float GetElapsedGameTime() const { return mElapsedGameTime; }

		/// <summary>
		/// Gets the elapsed real time, in seconds.
		/// </summary>
		/// <value>The elapsed real time.</value>
		float GetElapsedRealTime() const { return mElapsedRealTime; }

		/// <summary>
		/// Gets the total game time, in seconds.
		/// </summary>
		/// <value>The total game time.</value>
		float GetTotalGameTime() const { return mTotalGameTime; }


		/// <summary>
		/// Gets the total real time, in seconds.
		/// </summary>
		/// <value>The total real time.</value>
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
		/// </value>
		bool IsRunningSlowly() const { return mIsRunningSlowly; }

		
	private:
	
		float mFramesPerSecond;
		float mElapsedGameTime;
		float mElapsedRealTime;
		float mTotalGameTime;
		float mTotalRealTime;
		float mFramesPerSecond;

		bool mIsRunningSlowly;
		
		
		
	};
}

#endif