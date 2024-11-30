---
title: Improve documentation
sidebar_position: 1
---

# Improve documentation

Want to help improve this documentation?
Thank you for your interest in making it better!

Here’s a quick overview to get you started.

## Fix typo and/or complete an existing page

Each pages of the Guide including this one provides a link at its whole bottom:

[Edit this page](hhttps://github.com/NSPManager/NSPanelManager/edit/beta/docs/docs/contribute/documentation/improve-documentation.md)

1. Simply click over this link, it will allow you to edit via GitHub UI directly, the content of the page.
2. Update the content using Markdown in the editor.
3. Check that it renders well using the **Preview** tab
4. Click on the button **Commit changes...**
5. Click on the button **Create pull request**
6. Bingo, you did your first contribution, it will be available as soon as the project maintainers reviewed your changes.

## Create a new page

1. Fork the project
2. Create a new file ending with `.md` within `docs/docs/` directory.
3. Your file content should follow the following structure:

   ```markdown
   ---
   title: <page title>
   sidebar_position: <order in sidebar menu>
   ---
   
   # <page title>
   
   <your content>
   ```
   
4. You can, make use of MDX components (rich UI components) by importing them within the pages.
   Have a look at this to learn more: https://docusaurus.io/docs/markdown-features.

5. Optionally, you might also want to use specific widgets created for this project. 
   They are documented here: [custom widgets](./custom-widgets.md)
## Customize sidebar menu

Edit `docs/src/sidebars.ts`

```ts
const sidebars: SidebarsConfig = {
    mainSidebar: [
        {
            type: 'category',
            label: 'Getting started',
            items: [
                'getting-started/pre-requisites',
                'getting-started/supported-devices',
                'getting-started/flash-nspanel',
                {
                    type: 'category',
                    label: 'Install NSPanel Manager',
                    items: [
                        {
                            type: 'autogenerated',
                            dirName: 'getting-started/install',
                        },
                    ],
                },
                'getting-started/configuration',
            ]
        },
        'web-interface',
        'panel-functions',
        {
            type: 'category',
            label: 'Advanced topics',
            items: [
                {
                    type: 'autogenerated',
                    dirName: 'advanced',
                },
            ]
        },
        {
            type: 'category',
            label: 'Contribute',
            items: [
                {
                    type: 'autogenerated',
                    dirName: 'contribute',
                },
            ]
        },
    ],
};
```

More help is available on docusaurus: https://docusaurus.io/docs/sidebar