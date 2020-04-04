<p align="center">
    <img alt="skywriter" src="https://github.com/cengels/skywriter/blob/master/src/main/resources/com/cengels/skywriter/air-banner.png">
</p>

<h3 align="center">
    A distraction-free writer for writing books in Markdown
</h3>

<br />

## Why Skywriter?

There are many really cool distraction-free writing programs out there. However, most export only to proprietary rich-text formats such as .doc, .odt, or .rtf. This is not optimal for many reasons.

### Too Much Information

Most of those applications *hide away* the formatting information behind a minimalistic facade, but it's **still stored inside the file.** Skywriter uses the minimalistic format Markdown, which stores only semantics, not style. In other words: it saves **what** you want, not **how** you want it. If you're writing a manuscript, you don't really want to bother with setting a font size, making sure paragraph indentations and spacings are consistent throughout, and so on. Skywriter doesn't store concrete styles like font size or text color. It only specifies that something should be italicized, or that something should be a heading.

### Program-dependent

Formats like .odt and .doc are **program-dependent**, meaning that they can only be viewed and edited in certain programs because those formats store their text in binary format. Markdown is a plain text format, which means that you **don't rely on Skywriter** to look at and edit your text. If, at some point down the line, you decide to no longer use Skywriter, any simple text editor (including Notepad) will serve to view and edit your manuscript.

### Version Control

Storing a backup of a file is easy and works with any file format. However, this generally only stores the *last* version of the file (or all versions in the last n days). A version control software like git stores **every version of the file**, which is especially useful for novel writing. You don't need to manually store your alternate versions anymore. You want to delete an entire array of chapters? No problem. Then, later down the line, you realize you actually need the content of those chapters after all? Again, no problem. All versions are retained.

The problem: version control works best with plain text files. It allows you to directly compare two files for differences, which is a very useful feature. Want to check what changed between two versions? Version control can do that.

### Novel Writing

Skywriter is specifically designed for writers of manuscripts and supports a number of useful features like its own word tracker that tracks all the words you've written per session (and even makes sure that deleting previous sections during editing phases does not negatively influence your word count) along with the time needed for it while taking out the breaks you took. See below for a full list of features.

## Feature List (WIP)

- [x] Import from Markdown
- [x] Export to Markdown
- [x] Basic formatting
- [x] Theming ([#2](https://github.com/cengels/skywriter/issues/2))
- [x] Smart quotes ([#4](https://github.com/cengels/skywriter/issues/4))
- [x] Find and replace ([#5](https://github.com/cengels/skywriter/issues/5))
- [ ] Spellcheck ([#7](https://github.com/cengels/skywriter/issues/7))
- [x] Tracker for words written per day ([#9](https://github.com/cengels/skywriter/issues/9))
- [x] Tracker for time written per day ([#10](https://github.com/cengels/skywriter/issues/10))

## Download

**Skywriter is still under construction!** You can track the progress under the issues tab above. Once the first alpha version is ready, it will be available under this section for download.
