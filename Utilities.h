#pragma once

//storing the indices of the location of the queue family (if exist)
//REM: each queue family is given an index which defines where its location is on GPU

struct QueueFamilyIndices {
	int graphicsFamily = -1;			//Location of graphics family
	
	//check if queue famlies are valid
	bool isValid() {
		return graphicsFamily >= 0;
	}
};