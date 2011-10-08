var isExtended = 0;
var height = 1200;
var width = 200;
var slideDuration = 500;
var opacityDuration = 1000;

function extendContract(){
	
	if(isExtended == 0){
		
		sideBarSlide(0, height, 0, width);
		
		sideBarOpacity(0, 1);
	
		isExtended = 1;
		
		// make expand tab arrow image face left (inwards)
		$('sideBarTab').childNodes[0].src = $('sideBarTab').childNodes[0].src.replace(/(\.[^.]+)$/, '-active$1');
		
	}
	else{
		
		sideBarSlide(height, 0, width, 0);
		
		sideBarOpacity(1, 0);
		
		isExtended = 0;
		
		// make expand tab arrow image face right (outwards)
		
		$('sideBarTab').childNodes[0].src = $('sideBarTab').childNodes[0].src.replace(/-active(\.[^.]+)$/, '$1');
	}

}

function sideBarSlide(fromHeight, toHeight, fromWidth, toWidth){
		var myEffects = new Fx.Morph('sideBarContents', {duration: slideDuration, transition: 'quint:out'});
		myEffects.start({
			 'height': [fromHeight, toHeight],
			 'width': [fromWidth, toWidth]
		});
}

function sideBarOpacity(from, to){
		var myEffects = new Fx.Morph('sideBarContents', {duration: opacityDuration, transition: 'quint:out'});
		myEffects.start({
			 'opacity': [from, to]
		});
}

function init(){
	$('sideBarTab').addEvent('click', function(){extendContract()});
}

window.addEvent('load', function(){init()});