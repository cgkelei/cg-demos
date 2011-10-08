
window.addEvent('domready', function(){
	// We are setting the opacity of the element to 0.5 and adding two events
	$$('#chad_navigator_list li a').each(function(element){
        var morph = new Fx.Morph(element,{ duration:'300', link: 'cancel' });
		element.addEvents({
			mouseenter: function(){
				// This morphes the opacity and backgroundColor
				morph.start({
					'background-color': '#385d9b',
					'color': '#ffffff'
				});
			},
			mouseleave: function(){
				// Morphes back to the original style
				morph.start({
					'background-color': '#1a1a1a',
                    'color': '#cccccc'
				});
			}
		});
	});
	
	$$('#sideBarContentsInner li a').each(function(element){
        var morph = new Fx.Morph(element,{ duration:'300', link: 'cancel' });
		element.addEvents({
			mouseenter: function(){
				// This morphes the opacity and backgroundColor
				morph.start({
					'background-color': '#385d9b',
					'color': '#ffffff'
				});
			},
			mouseleave: function(){
				// Morphes back to the original style
				morph.start({
					'background-color': '#1a1a1a',
                    'color': '#cccccc'
				});
			}
		});
	});
}); 

function toggleDisplay( toggleNum ) 
{
    // Get the div and image
    var div = document.getElementById( 'div' + toggleNum );
    var img = document.getElementById( 'img' + toggleNum );
    var value;
    if ( div.style.display == '' )
    {
        div.style.display = 'none';
        img.setAttribute( 'src', '/images/plus.jpg' );
    }
    else
    {
        div.style.display = '';
        img.setAttribute( 'src', '/images/minus.jpg' );
    }
}
		
 