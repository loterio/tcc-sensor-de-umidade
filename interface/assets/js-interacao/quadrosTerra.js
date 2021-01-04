const rangeInput = document.getElementById('umidade-ideal');
const rangeOutput = document.getElementById('valor-umidade-ideal');

rangeInput.oninput = function () {
  rangeOutput.innerHTML = this.value;
}

function iniciaModal(idModal) {
  const backgroundModal = document.querySelector(".background-modal");
  const modal = document.getElementById(idModal);
  if (modal) {
    backgroundModal.classList.add('mostrar');
    modal.classList.add('mostrar');
    backgroundModal.addEventListener('click', (e) => {
      console.log(e.target.className);
      if (e.target.className == 'background-modal mostrar' || e.target.className == 'fechar icone') {
        backgroundModal.classList.remove('mostrar');
        modal.classList.remove('mostrar');
      }
    })
  }
}

mapboxgl.accessToken = 'pk.eyJ1IjoiaGVucmlxdWViIiwiYSI6ImNrZ2QwcDJiaTA4dWMydHBlajNxbnpsMjAifQ.C3dVATUIc81b2v4S57j6aw';

let map = new mapboxgl.Map({
  container: 'map', // container id
  style: 'mapbox://styles/mapbox/satellite-streets-v11', //hosted style id
  center: [-49.682295, -27.352328], // starting position
  zoom: 16 // starting zoom
});

let draw = new MapboxDraw({
  displayControlsDefault: false,
  controls: {
    trash: true,
  }
});

let localizacao = new mapboxgl.GeolocateControl({
  positionOptions: {
    enableHighAccuracy: false
  },
  trackUserLocation: false
})

map.addControl(localizacao);
map.addControl(draw);
draw.changeMode('draw_polygon');

let simpleSelect = false;
map.on('draw.create', salvaCoords);
map.on('draw.update', salvaCoords);
map.on('draw.modechange', () => {
  if (!simpleSelect)
    draw.changeMode('draw_polygon');
});
map.on('draw.delete', () => {
  draw.changeMode('draw_polygon');
});

function salvaCoords(quadro) {
  simpleSelect = true
  let coords = quadro.features[0].geometry.coordinates;
  coordsString = "";
  coords[0].forEach(coordenada => {
    // console.log(coordenada);
    coordsString += coordenada[0];
    coordsString += "|";
    coordsString += coordenada[1];
    coordsString += "|";
  });
  document.querySelector("[name='coordenadas-terreno']").value = coordsString;
}
