# Generated by Django 4.1.7 on 2023-03-28 22:00

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('web', '0018_alter_nspanel_status_data'),
    ]

    operations = [
        migrations.AddField(
            model_name='nspanel',
            name='online_state',
            field=models.BooleanField(default=False),
        ),
    ]