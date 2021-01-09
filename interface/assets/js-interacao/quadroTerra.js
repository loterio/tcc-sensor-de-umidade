mapboxgl.accessToken = 'pk.eyJ1IjoiaGVucmlxdWViIiwiYSI6ImNrZ2QwcDJiaTA4dWMydHBlajNxbnpsMjAifQ.C3dVATUIc81b2v4S57j6aw';


// fetch('quadroTerra.json')
//   .then(res => res.json())
//   .then(dados => carregaMapas(dados));

let coordenadas = JSON.parse('["-49.68197983142528","-27.35206738398432","-49.68197222690114","-27.352290276943847","-49.6824361028759","-27.352394968936153","-49.682333441799244","-27.35307039876944","-49.68110911340639","-27.352675272816974","-49.68129162198616","-27.35186475363154","-49.68197983142528","-27.35206738398432"]');
carregaMapas(coordenadas);


//============Funções=====================
function carregaMapas(dados) {
  let coordsQuadro = [];
  for (let i = 0; i < dados.length; i += 2) {
    let y = i + 1;
    coordsQuadro.push([Number(dados[i]), Number(dados[y])]);
  }

  let quadroTerra = turf.polygon([coordsQuadro]);
  let centerGeo = turf.center(quadroTerra);
  let pontoCentral = centerGeo.geometry.coordinates;


  let map = new mapboxgl.Map({
    container: 'map', // container id
    style: 'mapbox://styles/mapbox/satellite-streets-v11', //hosted style id
    center: pontoCentral, // starting position
    zoom: 18 // starting zoom
  });

  map.on('load', function () {
    map.addSource('maine', {
      'type': 'geojson',
      'data': {
        'type': 'Feature',
        'geometry': {
          'type': 'Polygon',
          'coordinates': [
            coordsQuadro
          ]
        }
      }
    });
    map.addLayer({
      'id': 'maine',
      'type': 'fill',
      'source': 'maine',
      'layout': {},
      'paint': {
        'fill-color': '#088',
        'fill-opacity': 0.5
      }
    });
  });


  let pontos = JSON.parse('[["-49.681380326240884","-27.352004134578173"]]');
  carregaPontos(map, quadroTerra, pontos);
}

function carregaPontos(map, quadroTerra, dados) {
  dados.forEach(ponto => {
    let pt = turf.point(ponto);
    if(turf.booleanPointInPolygon(pt, quadroTerra)){
      let marker = new mapboxgl.Marker()
        .setLngLat(ponto)
        .addTo(map);
    }
    const marcadoresContainer = document.querySelector(".mapboxgl-canvas-container");
    const marcadores = document.querySelectorAll(".mapboxgl-marker");
    const marcadorAdicionado = marcadores[marcadores.length - 1];
    marcadoresContainer.removeChild(marcadores[marcadores.length - 1]);

    const link = document.createElement("a");
    link.setAttribute("href", "#id-sensor-1")
    link.insertAdjacentElement("beforeend", marcadorAdicionado);
    marcadoresContainer.insertAdjacentElement("beforeend", link);
    console.log(link);
  });
}