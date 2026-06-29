# MTG Custom Set Viewer

### What is it?
A tool designed to allow the viewing of custom Magic the Gathering decks. It is a visual library of your custom MTG decks.

### What is it not?
It is not an MTG game. It is not a tool for making MTG cards or decks. It is purely for viewing existing made decks.

### What do I need?
A copy of the latest release of this application, a Cockatrice V2.2 XML file, and either a) the images for the cards already prepared such as with a tool like MSE or, b) a URL to the image online such as with a tool like mtg.design. That's it.

### How do I use it?
First, go to releases and download the latest Windows release.

Second, extract the contents to wherever you want.

Third, in the same directory as the executable, there is a `Sets` directory. Simply drop a custom Cockatrice compliant deck in that directory adhering to the following format:
```
Sets
- SetName [Dir]
- - SetName-files [Dir]
- - SetName.xml [File]
- OtherSetName [Dir]
- - OtherSetName-files [Dir]
- - OtherSetName.xml [File]
```
This format is based on MSE(Magic Set Editor)'s format when exporting to Cockatrice V2.2 with `Export Images` enabled.

### Some of my cards are blank!
Verify that the name of your image files matches that of your card names in the corresponding XML.
