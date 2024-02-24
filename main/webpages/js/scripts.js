/*!
    * Start Bootstrap - SB Admin v7.0.0 (https://startbootstrap.com/template/sb-admin)
    * Copyright 2013-2021 Start Bootstrap
    * Licensed under MIT (https://github.com/StartBootstrap/startbootstrap-sb-admin/blob/master/LICENSE)
    */
    // 
// Scripts
// 

window.addEventListener('DOMContentLoaded', event => {

    // Toggle the side navigation
    const sidebarToggle = document.body.querySelector('#sidebarToggle');
    if (sidebarToggle) {
        // Uncomment Below to persist sidebar toggle between refreshes
        // if (localStorage.getItem('sb|sidebar-toggle') === 'true') {
        //     document.body.classList.toggle('sb-sidenav-toggled');
        // }
        sidebarToggle.addEventListener('click', event => {
            event.preventDefault();
            document.body.classList.toggle('sb-sidenav-toggled');
            localStorage.setItem('sb|sidebar-toggle', document.body.classList.contains('sb-sidenav-toggled'));
        });
    }

});

function setOutput(form)
{
    var mode = form.mode.value;
    fetch("/set", {
        method: "POST",
        body: "mode=" + mode
    })
    .then((response) => {
        if (response.ok) {
          return response.text();
        }
        return Promise.reject(response);
    })
    .catch((error) => {
        alert("Failed to set output: " + error)
        console.error("Failed to set output: " + error)
    })
}

function setOutputSettings(form)
{
    var brightness = form.brightness.value;
    var idleTimeout = form.idleTimeout.value;
    var idleMode = form.idleMode.value;
    fetch("/settings", {
        method: "POST",
        body: "brightness=" + brightness + "&idleTimeout=" + idleTimeout + "&idleMode=" + idleMode
    })
    .then((response) => {
        if (response.ok) {
          return response.text();
        }
        return Promise.reject(response);
    })
    .catch((error) => {
        alert("Failed to set settings: " + error)
        console.error("Failed to set settings: " + error)
    })
}

function setArtnetSettings(form)
{
    var Net = form.Net.value;
    var Subnet = form.Subnet.value;
    var Universe = form.Universe.value;
    var DMXAddr = form.DMXAddr.value;
    fetch("/artnetConfig", {
        method: "POST",
        body: "Net=" + Net + "&Subnet=" + Subnet + "&Universe=" + Universe + "&DMXAddr=" + DMXAddr
    })
    .then((response) => {
        if (response.ok) {
          return response.text();
        }
        return Promise.reject(response);
    })
    .catch((error) => {
        alert("Failed to set address: " + error)
        console.error("Failed to set address: " + error)
    })
}

function setLocator(form)
{
    var locate = form.locate.value;
    fetch("/locate", {
        method: "POST",
        body: "locate=" + locate
    })
    .then((response) => {
        if (response.ok) {
          return response.text();
        }
        return Promise.reject(response);
    })
    .catch((error) => {
        alert("Failed to set locator: " + error)
        console.error("Failed to set locator: " + error)
    })
}