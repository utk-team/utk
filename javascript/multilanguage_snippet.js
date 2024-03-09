function openCode(codeName, elmnt, color) 
{
    var defaultColor = getComputedStyle(elmnt).getPropertyValue("--md-primary-fg-color");
    // Hide all elements with class="tabcontent" by default */
    var i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("tabcontent");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }

    // Remove the background color of all tablinks/buttons
    tablinks = document.getElementsByClassName("tablink");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].style.backgroundColor = "";
        tablinks[i].style.color = "black";
        tablinks[i].style.fontWeight = "normal";
    }

    // Show the specific tabs content
    var elements = document.getElementsByClassName(codeName);
    for (i = 0; i < elements.length; i++) {
        elements[i].style.display = "block";
    }

    // Add the specific color to the button used to open the tab content
    elements = document.getElementsByClassName(elmnt.className);
    for (i = 0; i < elements.length; i++) {
        elements[i].style.backgroundColor = defaultColor;
        elements[i].style.color = "black";
        elements[i].style.fontWeight = "bold";
    }
}

// Get the element with id="defaultOpen" and click on it
if (document.getElementsByClassName("exebutton") !== null) {
    var elements = document.getElementsByClassName("exebutton");
    for (i = 0; i < elements.length; i++) {
        elements[i].click();
    }
}