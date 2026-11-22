import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import YoutubePlayer from '@site/src/components/YoutubePlayer';

import styles from './index.module.css';
import React from "react";

function HomepageHeader() {
  return (
    <header className={clsx('hero hero--primary', styles.heroBanner)}>
        <div className="container">
            <img
                src="/images/logo-dark.svg"
                alt="NSPanel Manager logo"
                style={{maxWidth: '100%', margin: '1rem auto', borderRadius: '8px'}}
            />
            <div className={styles.buttons}>
                <Link
                    className="button button--secondary button--lg"
                    to="/docs/getting-started/pre-requisites">
                    Getting started
                </Link>
            </div>
        </div>
    </header>
  );
}

export default function Home(): JSX.Element {
    const {siteConfig} = useDocusaurusContext();
    return (
        <Layout
            title={`Hello from ${siteConfig.title}`}
      description="Description will go into a meta tag in <head />">
      <HomepageHeader />
            <main>
                <YoutubePlayer
                    videoId="ko5_FA-TdAU"
                    author="Cables & Coffee (co-founder)"
                    description="Quickly see what you can achieve!"
                />
            </main>
        </Layout>
);
}
