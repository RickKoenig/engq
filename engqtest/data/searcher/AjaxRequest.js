/* Check browser type and create ajax request object
Put this function in an external .js file and use it for your
Ajax programs. Fully explained in Example 18.1. */
function createRequest(){
	var ajaxRequest; // The variable that makes Ajax possible!
	try{
		// Opera 8.0+, Firefox, Safari
		ajaxRequest = new XMLHttpRequest(); // Create the object
	}catch (e){
		// Internet Explorer Browsers
		try{
			ajaxRequest = new ActiveXObject("Msxml2.XMLHTTP");
		}catch (e){
			try{
				ajaxRequest = new ActiveXObject("Microsoft.XMLHTTP");
			}catch (e){
				return false;
			}
		}
	}
	return ajaxRequest;
} //End function