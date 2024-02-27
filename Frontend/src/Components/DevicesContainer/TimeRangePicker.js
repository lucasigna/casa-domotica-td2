import React, { useState } from "react";

const TimeRangePicker = ({isScheduled, startTime, endTime, setStartTime, setEndTime, setChanges}) => {

  const handleStartTimeChange = (event) => {
    setStartTime(event.target.value);
    setChanges(true)
  };

  const handleEndTimeChange = (event) => {
    setEndTime(event.target.value);
    setChanges(true)
  };



  return (
    <div className="timeRangeContainer" style={{opacity: `${isScheduled ? '100%' : '50%'}`}}>
      <div className="fromContainer">
        <label>Desde</label>
        <input
          type="time"
          value={startTime}
          onChange={handleStartTimeChange}
          disabled={!isScheduled}
        />
      </div>
      <div className="toContainer">
        <label>Hasta</label>
        <input
          type="time"
          value={endTime}
          onChange={handleEndTimeChange}
          disabled={!isScheduled}
        />
      </div>
    </div>
  );
};

export default TimeRangePicker;
