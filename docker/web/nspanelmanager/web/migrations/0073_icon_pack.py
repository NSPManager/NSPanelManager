from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ("web", "0072_make_settings_unique"),
    ]

    operations = [
        migrations.AddField(
            model_name="scene",
            name="icon",
            field=models.CharField(blank=True, default="", max_length=8),
        ),
        migrations.CreateModel(
            name="IconPackIcon",
            fields=[
                ("id", models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name="ID")),
                ("name", models.CharField(max_length=128, unique=True)),
                ("code_point", models.IntegerField(unique=True)),
            ],
        ),
    ]
