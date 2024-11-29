---
title: Custom widgets
sidebar_position: 2
---
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';
import CenteredImage from '@site/src/components/CenteredImage';
import Centered from '@site/src/components/Centered';
import YouTubePlayer from '@site/src/components/YoutubePlayer';
import Label from '@site/src/components/Label';

# Custom widgets

This page demonstrate the custom widgets available in this project.

:::info
All such widgets are stored within `docs/src/components` directory.
:::

## Label - Rounded tags

Widget to add labels such as beta features.

<Tabs>
  <TabItem value="code" label="Code">
    **Import**

    Add import line at the top of the markdown file:
    ```markdown
    import Label from '@site/src/components/Label';
    ```
    
    **Use**
    
    Place the content to place a Label in your markdown page:
    
    ```markdown
        <Label value="BETA"/> feature
    ```
  </TabItem>
  <TabItem value="preview" label="Preview" default>
    <Label value="BETA"/> feature
  </TabItem>
</Tabs>

## CenteredImage - Centered image

Widget to center an image on the page.

<Tabs>
  <TabItem value="code" label="Code">
    **Import**
    
    Add import line at the top of the markdown file:
    ```markdown
    import CenteredImage from '@site/src/components/CenteredImage';
    ```
    
    **Use**
    
    Place the content to center in your markdown page:
    
    ```markdown
    <CenteredImage 
        src="/images/doc/initial_setup_popup.png"
        alt="Configuration wizard"
        figureNumber={1} 
    />
    ```
  </TabItem>
  <TabItem value="preview" label="Preview" default>
    <CenteredImage 
        src="/images/doc/initial_setup_popup.png" 
        alt="Configuration wizard" 
        figureNumber={1} 
    />
  </TabItem>
</Tabs>


## Centered - Centered content

Widget to center content within the page.

<Tabs>
  <TabItem value="code" label="Code">
    **Import**

    Add import line at the top of the markdown file:
    ```markdown
    import Centered from '@site/src/components/Centered';
    ```
    
    **Use**
    
    Place the content to center in your markdown page:
    
    ```markdown
    <Centered>
       Some central text
    </Centered>
    ```
  </TabItem>
  <TabItem value="preview" label="Preview" default>
    <Centered>
       Some central text
    </Centered>
  </TabItem>
</Tabs>

## YoutubePlayer - Embed Youtube videos

Widget to display YouTube videos.

<Tabs>
  <TabItem value="code" label="Code">
    **Import**

    Add import line at the top of the markdown file:
    ```markdown
    import YoutubePlayer from '@site/src/components/YoutubePlayer';
    ```
    
    **Use**
    
    Place the content to place a Youtube player in your markdown page:
    
    ```markdown
        <YouTubePlayer
            videoId="ko5_FA-TdAU"
            author="Cables & Coffee (co-founder)"
            description="Quickly see what you can achieve!"
        />
    ```
  </TabItem>
  <TabItem value="preview" label="Preview" default>
    <YouTubePlayer
        videoId="ko5_FA-TdAU"
        author="Cables & Coffee (co-founder)"
        description="Quickly see what you can achieve!"
    />
  </TabItem>
</Tabs>